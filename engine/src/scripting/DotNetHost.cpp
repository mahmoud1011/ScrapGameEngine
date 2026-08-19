#include "scripting/DotNetHost.h"

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>

#ifdef _WIN32
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  using char_t_str = std::wstring;
  #define STR(s) L##s
#else
  #include <dlfcn.h>
  using char_t_str = std::string;
  #define STR(s) s
#endif

using namespace ScrapGameEngine;

namespace
{
    hostfxr_initialize_for_runtime_config_fn initForConfig = nullptr;
    hostfxr_get_runtime_delegate_fn getDelegate = nullptr;
    hostfxr_close_fn closeHost = nullptr;

    load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFn = nullptr;
    hostfxr_handle hostContext = nullptr;

    void* hostfxrLib = nullptr;
    bool ready = false;
    std::string description = "not loaded";
    char_t_str managedAssemblyPath;

    /** Converts a UTF-8 path to whatever width the host API wants on this platform. */
    char_t_str toHostString(const std::string& s)
    {
#ifdef _WIN32
        if (s.empty()) return {};
        const int needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
        std::wstring out(static_cast<size_t>(needed - 1), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, out.data(), needed);
        return out;
#else
        return s;
#endif
    }

    void* loadLibraryPortable(const char_t_str& path)
    {
#ifdef _WIN32
        return static_cast<void*>(::LoadLibraryW(path.c_str()));
#else
        return dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
    }

    void* getSymbol(void* lib, const char* name)
    {
#ifdef _WIN32
        return reinterpret_cast<void*>(::GetProcAddress(static_cast<HMODULE>(lib), name));
#else
        return dlsym(lib, name);
#endif
    }

    /** Locates hostfxr through nethost and binds the three entry points we need. */
    bool loadHostfxr()
    {
        if (hostfxrLib != nullptr) return true;

        char_t buffer[1024];
        size_t size = sizeof(buffer) / sizeof(char_t);

        const int rc = get_hostfxr_path(buffer, &size, nullptr);
        if (rc != 0)
        {
            std::cerr << "[DOTNET] hostfxr not found (0x" << std::hex << rc << std::dec
                      << "). Is the .NET runtime installed?" << std::endl;
            return false;
        }

        hostfxrLib = loadLibraryPortable(char_t_str(buffer));
        if (hostfxrLib == nullptr)
        {
            std::cerr << "[DOTNET] could not load hostfxr." << std::endl;
            return false;
        }

        initForConfig = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
            getSymbol(hostfxrLib, "hostfxr_initialize_for_runtime_config"));
        getDelegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
            getSymbol(hostfxrLib, "hostfxr_get_runtime_delegate"));
        closeHost = reinterpret_cast<hostfxr_close_fn>(
            getSymbol(hostfxrLib, "hostfxr_close"));

        return initForConfig && getDelegate && closeHost;
    }
}

bool DotNetHost::initialize(const std::string& runtimeConfigPath, const std::string& assemblyPath)
{
    if (ready) return true;

    namespace fs = std::filesystem;
    if (!fs::exists(runtimeConfigPath) || !fs::exists(assemblyPath))
    {
        std::cerr << "[DOTNET] scripting assembly not found - C# is disabled.\n"
                  << "         config:   " << runtimeConfigPath << "\n"
                  << "         assembly: " << assemblyPath << std::endl;
        return false;
    }

    if (!loadHostfxr()) return false;

    const char_t_str config = toHostString(runtimeConfigPath);
    int rc = initForConfig(config.c_str(), nullptr, &hostContext);

    // Success is 0; 1 and 2 are "already initialised" variants that still hand back a
    // usable context, so they are not failures.
    if ((rc != 0 && rc != 1 && rc != 2) || hostContext == nullptr)
    {
        std::cerr << "[DOTNET] runtime init failed (0x" << std::hex << rc << std::dec << ")." << std::endl;
        if (hostContext) { closeHost(hostContext); hostContext = nullptr; }
        return false;
    }

    void* raw = nullptr;
    rc = getDelegate(hostContext, hdt_load_assembly_and_get_function_pointer, &raw);
    if (rc != 0 || raw == nullptr)
    {
        std::cerr << "[DOTNET] could not get the load-assembly delegate (0x"
                  << std::hex << rc << std::dec << ")." << std::endl;
        closeHost(hostContext);
        hostContext = nullptr;
        return false;
    }

    loadAssemblyAndGetFn = reinterpret_cast<load_assembly_and_get_function_pointer_fn>(raw);
    managedAssemblyPath = toHostString(assemblyPath);
    description = "CoreCLR via hostfxr";
    ready = true;

    std::cout << "[DOTNET] runtime hosted; scripting assembly "
              << fs::path(assemblyPath).filename().string() << " ready." << std::endl;
    return true;
}

void* DotNetHost::getFunction(const std::string& typeName, const std::string& methodName)
{
    if (!ready) return nullptr;

    const char_t_str type = toHostString(typeName);
    const char_t_str method = toHostString(methodName);

    void* fn = nullptr;
    // UNMANAGEDCALLERSONLY_METHOD means the managed side is [UnmanagedCallersOnly] and
    // needs no delegate type - the cheapest boundary the runtime offers.
    const int rc = loadAssemblyAndGetFn(
        managedAssemblyPath.c_str(), type.c_str(), method.c_str(),
        UNMANAGEDCALLERSONLY_METHOD, nullptr, &fn);

    if (rc != 0 || fn == nullptr)
    {
        std::cerr << "[DOTNET] could not resolve " << typeName << "::" << methodName
                  << " (0x" << std::hex << rc << std::dec << ")." << std::endl;
        return nullptr;
    }
    return fn;
}

void DotNetHost::shutdown()
{
    if (hostContext != nullptr)
    {
        closeHost(hostContext);
        hostContext = nullptr;
    }
    loadAssemblyAndGetFn = nullptr;
    ready = false;
    description = "not loaded";
}

bool DotNetHost::isReady() { return ready; }
const std::string& DotNetHost::runtimeDescription() { return description; }
