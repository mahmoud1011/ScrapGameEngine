#pragma once

#include <string>

namespace ScrapGameEngine
{
    /**
     * @class DotNetHost
     * @brief Hosts CoreCLR in-process so game code can be written in modern C#.
     *
     * Uses the official hosting chain - nethost to locate hostfxr, hostfxr to start a
     * runtime from a runtimeconfig.json, then load_assembly_and_get_function_pointer to
     * reach managed entry points. That is the same path .NET itself documents, and the
     * direction Unity is moving; it is not Mono.
     *
     * Why CoreCLR rather than NativeAOT: user scripts must be loadable and *unloadable*
     * at runtime for hot reload. NativeAOT compiles ahead of time and cannot do that.
     * The collectible AssemblyLoadContext is the whole point.
     *
     * Interop shape: calls cross as plain function pointers with blittable arguments.
     * No reflection on the hot path, no marshalling of managed types into native code -
     * a managed entity is an integer handle, not an object the engine holds.
     */
    class DotNetHost
    {
    public:
        DotNetHost() = delete;

        /**
         * @brief Starts the runtime and loads the scripting assembly.
         * @param runtimeConfigPath Path to the assembly's .runtimeconfig.json.
         * @param assemblyPath Path to the managed .dll.
         * @return False if the runtime could not be located or started; the engine
         *         continues without scripting rather than failing to launch.
         */
        static bool initialize(const std::string& runtimeConfigPath,
                               const std::string& assemblyPath);

        static void shutdown();

        /** @brief True once a runtime is up and the assembly is loaded. */
        static bool isReady();

        /**
         * @brief Resolves a managed static method to a callable function pointer.
         * @param typeName Assembly-qualified type, e.g. "Scrap.Bootstrap, ScrapScript".
         * @param methodName Method marked [UnmanagedCallersOnly].
         * @return Function pointer, or nullptr if resolution failed.
         *
         * The caller casts to the matching signature. Getting that wrong corrupts the
         * stack, so signatures live next to their managed declarations by convention.
         */
        static void* getFunction(const std::string& typeName, const std::string& methodName);

        /** @brief The runtime version actually loaded, for the editor's about box. */
        static const std::string& runtimeDescription();
    };
}
