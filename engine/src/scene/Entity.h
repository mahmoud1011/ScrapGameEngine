#pragma once

#include "scene/Scene.h"

#include <entt/entt.hpp>
#include <utility>

namespace Scrap
{
    /**
     * @class Entity
     * @brief A handle into a Scene's registry.
     *
     * Deliberately a small value type, not an object that owns anything - copying one
     * is copying a pair of pointers-worth of data, and it stays valid only as long as
     * its Scene does.
     *
     * entt:: types are confined to this header and Scene's private section, which is
     * what keeps the ECS swappable later. The plan calls this out as the thing most
     * likely to leak.
     */
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : handle(handle), scene(scene) {}

        template<typename T, typename... Args>
        T& addComponent(Args&&... args)
        {
            return scene->registry.emplace<T>(handle, std::forward<Args>(args)...);
        }

        /** @brief Adds, or overwrites if already present. */
        template<typename T, typename... Args>
        T& addOrReplaceComponent(Args&&... args)
        {
            return scene->registry.emplace_or_replace<T>(handle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& getComponent()
        {
            return scene->registry.get<T>(handle);
        }

        template<typename T>
        bool hasComponent() const
        {
            return scene->registry.all_of<T>(handle);
        }

        template<typename T>
        void removeComponent()
        {
            scene->registry.remove<T>(handle);
        }

        /** @brief The component, or nullptr when absent - avoids a has/get pair. */
        template<typename T>
        T* tryGetComponent()
        {
            return scene->registry.try_get<T>(handle);
        }

        UUID getUUID() { return getComponent<IDComponent>().id; }
        const std::string& getName() { return getComponent<TagComponent>().tag; }

        bool isValid() const { return scene != nullptr && scene->registry.valid(handle); }
        operator bool() const { return isValid(); }
        operator entt::entity() const { return handle; }
        operator uint32_t() const { return static_cast<uint32_t>(handle); }

        bool operator==(const Entity& other) const
        {
            return handle == other.handle && scene == other.scene;
        }
        bool operator!=(const Entity& other) const { return !(*this == other); }

        Scene* getScene() const { return scene; }

    private:
        entt::entity handle{entt::null};
        Scene* scene = nullptr;
    };
}
