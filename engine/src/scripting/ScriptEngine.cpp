#include "scripting/ScriptEngine.h"
#include "scripting/DotNetHost.h"

#include "scene/Entity.h"
#include "scene/Scene.h"

#include <cstring>
#include <iostream>
#include <vector>

namespace Scrap
{
    namespace
    {
        Scene* activeScene = nullptr;
        bool ready = false;
        int liveInstances = 0;

        // Managed entry points. Signatures must match the [UnmanagedCallersOnly]
        // declarations in ScrapScript.ScriptHost exactly; the runtime hands back a
        // raw pointer and validates nothing.
        using InitializeFn      = int  (*)(const ScriptInterop*);
        using CreateInstanceFn  = int  (*)(uint64_t entityId, const char* typeNameUtf8);
        using UpdateInstanceFn  = void (*)(uint64_t entityId, float deltaTime);
        using DestroyAllFn      = void (*)();

        CreateInstanceFn createInstance = nullptr;
        UpdateInstanceFn updateInstance = nullptr;
        DestroyAllFn destroyAll = nullptr;

        // --- native callbacks the managed side calls -------------------------
        // Every one resolves a UUID against the active scene rather than trusting a
        // pointer from managed code, so a stale id fails a lookup instead of
        // corrupting memory.

        TransformComponent* transformOf(uint64_t entityId)
        {
            if (activeScene == nullptr) return nullptr;
            Entity entity = activeScene->findByUUID(UUID(entityId));
            if (!entity) return nullptr;
            return entity.tryGetComponent<TransformComponent>();
        }

        void nativeLog(int level, const char* utf8)
        {
            const char* tag = level >= 2 ? "[C# ERROR]" : level == 1 ? "[C# WARN]" : "[C#]";
            (level >= 2 ? std::cerr : std::cout) << tag << ' '
                << (utf8 != nullptr ? utf8 : "") << std::endl;
        }

        bool nativeGetTranslation(uint64_t id, float* out3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || out3 == nullptr) return false;
            out3[0] = t->translation.x; out3[1] = t->translation.y; out3[2] = t->translation.z;
            return true;
        }

        bool nativeSetTranslation(uint64_t id, const float* in3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || in3 == nullptr) return false;
            t->translation = {in3[0], in3[1], in3[2]};
            return true;
        }

        bool nativeGetRotation(uint64_t id, float* out3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || out3 == nullptr) return false;
            out3[0] = t->rotation.x; out3[1] = t->rotation.y; out3[2] = t->rotation.z;
            return true;
        }

        bool nativeSetRotation(uint64_t id, const float* in3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || in3 == nullptr) return false;
            t->rotation = {in3[0], in3[1], in3[2]};
            return true;
        }

        bool nativeGetScale(uint64_t id, float* out3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || out3 == nullptr) return false;
            out3[0] = t->scale.x; out3[1] = t->scale.y; out3[2] = t->scale.z;
            return true;
        }

        bool nativeSetScale(uint64_t id, const float* in3)
        {
            auto* t = transformOf(id);
            if (t == nullptr || in3 == nullptr) return false;
            t->scale = {in3[0], in3[1], in3[2]};
            return true;
        }

        // Lives for the process: the managed side keeps the pointer it is handed.
        ScriptInterop interop{};
    }

    bool ScriptEngine::initialize()
    {
        if (ready) return true;
        if (!ScrapGameEngine::DotNetHost::isReady()) return false;

        const char* type = "Scrap.ScriptHost, ScrapScript";
        auto* init = reinterpret_cast<InitializeFn>(
            ScrapGameEngine::DotNetHost::getFunction(type, "Initialize"));
        createInstance = reinterpret_cast<CreateInstanceFn>(
            ScrapGameEngine::DotNetHost::getFunction(type, "CreateInstance"));
        updateInstance = reinterpret_cast<UpdateInstanceFn>(
            ScrapGameEngine::DotNetHost::getFunction(type, "UpdateInstance"));
        destroyAll = reinterpret_cast<DestroyAllFn>(
            ScrapGameEngine::DotNetHost::getFunction(type, "DestroyAll"));

        if (init == nullptr || createInstance == nullptr ||
            updateInstance == nullptr || destroyAll == nullptr)
        {
            std::cerr << "[SCRIPT] managed entry points missing - scripting disabled." << std::endl;
            return false;
        }

        interop.log = &nativeLog;
        interop.getTranslation = &nativeGetTranslation;
        interop.setTranslation = &nativeSetTranslation;
        interop.getRotation = &nativeGetRotation;
        interop.setRotation = &nativeSetRotation;
        interop.getScale = &nativeGetScale;
        interop.setScale = &nativeSetScale;

        if (init(&interop) != 0)
        {
            std::cerr << "[SCRIPT] managed Initialize reported failure." << std::endl;
            return false;
        }

        ready = true;
        std::cout << "[SCRIPT] managed script engine ready." << std::endl;
        return true;
    }

    void ScriptEngine::shutdown()
    {
        onRuntimeStop();
        createInstance = nullptr;
        updateInstance = nullptr;
        destroyAll = nullptr;
        ready = false;
    }

    bool ScriptEngine::isReady() { return ready; }
    Scene* ScriptEngine::currentScene() { return activeScene; }
    int ScriptEngine::liveInstanceCount() { return liveInstances; }

    void ScriptEngine::onRuntimeStart(Scene* scene)
    {
        activeScene = scene;
        liveInstances = 0;
        if (!ready || scene == nullptr) return;

        for (auto handle : scene->raw().view<ScriptComponent>())
        {
            Entity entity{handle, scene};
            const auto& script = entity.getComponent<ScriptComponent>();
            if (script.typeName.empty()) continue;

            const uint64_t id = entity.getUUID();
            if (createInstance(id, script.typeName.c_str()) == 0)
            {
                liveInstances++;
            }
            else
            {
                std::cerr << "[SCRIPT] could not instantiate '" << script.typeName
                          << "' on entity " << entity.getName() << std::endl;
            }
        }

        if (liveInstances > 0)
        {
            std::cout << "[SCRIPT] " << liveInstances << " managed instance(s) created." << std::endl;
        }
    }

    void ScriptEngine::onUpdate(float deltaTime)
    {
        if (!ready || activeScene == nullptr || liveInstances == 0) return;

        for (auto handle : activeScene->raw().view<ScriptComponent>())
        {
            Entity entity{handle, activeScene};
            if (entity.getComponent<ScriptComponent>().typeName.empty()) continue;
            updateInstance(entity.getUUID(), deltaTime);
        }
    }

    void ScriptEngine::onRuntimeStop()
    {
        if (ready && destroyAll != nullptr) destroyAll();
        liveInstances = 0;
        activeScene = nullptr;
    }
}
