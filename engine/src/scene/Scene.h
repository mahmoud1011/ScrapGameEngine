#pragma once

#include "scene/Components.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace Scrap
{
    class Entity;

    /**
     * @class Scene
     * @brief A world of entities, backed by an EnTT registry.
     *
     * D1. Replaces GameObject + GameObjectCollection: typed access is O(1), storage is
     * contiguous per component type, and iterate-by-component is the one mechanism the
     * renderer, the serializer and the inspector all use.
     *
     * A Scene is copyable so play mode can work on a snapshot and Stop can restore the
     * edit state without a reload.
     */
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity createEntity(const std::string& name = "Entity");
        Entity createEntityWithUUID(UUID id, const std::string& name = "Entity");
        void destroyEntity(Entity entity);

        /** @brief Finds an entity by UUID, or an invalid Entity if absent. */
        Entity findByUUID(UUID id);

        /** @brief Finds the first entity with this tag, or an invalid Entity. */
        Entity findByTag(const std::string& tag);

        /** @brief Advances scripts and systems. Only called in play mode. */
        void onUpdateRuntime(float deltaTime);

        /** @brief Draws the scene through the first primary CameraComponent. */
        void onRenderRuntime();

        /** @brief Draws the scene through a caller-supplied view-projection. */
        void onRenderEditor(const glm::mat4& viewProjection);

        /** @brief Deep copy, so play mode never mutates the authored scene. */
        static std::shared_ptr<Scene> copy(const std::shared_ptr<Scene>& source);

        entt::registry& raw() { return registry; }
        // Counted through IDComponent rather than the entity storage: every entity
        // this Scene creates has one, and the storage in_use() accessor is not
        // stable across EnTT versions.
        size_t entityCount() const { return registry.view<const IDComponent>().size(); }

        const std::string& getName() const { return name; }
        void setName(std::string value) { name = std::move(value); }

    private:
        friend class Entity;
        friend class SceneSerializer;

        void renderSprites();

        entt::registry registry;
        std::unordered_map<uint64_t, entt::entity> byUUID;
        std::string name = "Untitled";
    };
}
