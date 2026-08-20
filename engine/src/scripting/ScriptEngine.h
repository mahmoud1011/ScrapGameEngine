#pragma once

#include <cstdint>
#include <string>

namespace Scrap
{
    class Scene;
    class Entity;

    /**
     * @struct ScriptInterop
     * @brief The native functions the managed side may call.
     *
     * Passed once at startup as a table of plain function pointers. This is the entire
     * managed-to-native surface, and it is deliberately small: every entry takes
     * blittable arguments and an entity is a `uint64_t` UUID, never a pointer. Nothing
     * managed ever holds a native address, so a GC move or an assembly unload cannot
     * leave the engine with a dangling reference.
     *
     * Layout must match `Interop.NativeFunctions` in ScrapScript exactly. Field order
     * is the contract; adding one means adding it in both places, in the same position.
     */
    struct ScriptInterop
    {
        void (*log)(int level, const char* utf8);
        bool (*getTranslation)(uint64_t entityId, float* out3);
        bool (*setTranslation)(uint64_t entityId, const float* in3);
        bool (*getRotation)(uint64_t entityId, float* out3);
        bool (*setRotation)(uint64_t entityId, const float* in3);
        bool (*getScale)(uint64_t entityId, float* out3);
        bool (*setScale)(uint64_t entityId, const float* in3);
    };

    /**
     * @class ScriptEngine
     * @brief Binds ScriptComponent to managed instances on the hosted runtime.
     *
     * DotNetHost gets a runtime running; this is what gives it something to run. On
     * play it walks the scene, instantiates the managed type each ScriptComponent
     * names, and ticks them every frame until stop.
     */
    class ScriptEngine
    {
    public:
        ScriptEngine() = delete;

        /**
         * @brief Resolves the managed entry points and hands over the native table.
         * @return False when the runtime is unavailable; the engine then runs
         *         natively and scripted entities simply do nothing.
         */
        static bool initialize();
        static void shutdown();

        static bool isReady();

        /** @brief Instantiates a managed object for every scripted entity. */
        static void onRuntimeStart(Scene* scene);

        /** @brief Ticks every live managed instance. */
        static void onUpdate(float deltaTime);

        /** @brief Destroys all managed instances and drops the scene pointer. */
        static void onRuntimeStop();

        /** @brief How many managed instances are currently alive. */
        static int liveInstanceCount();

        /** @brief The scene the callbacks resolve entity ids against. */
        static Scene* currentScene();
    };
}
