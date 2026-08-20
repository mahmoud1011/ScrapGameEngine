#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace ScrapGameEngine { class Texture2D; class Mesh3D; }

namespace Scrap
{
    /**
     * @brief A stable 64-bit identity that survives save/load.
     *
     * entt::entity indices are recycled, so they cannot be written to disk. A UUID is
     * what a scene file references and what a prefab instance resolves against.
     */
    class UUID
    {
    public:
        UUID();                                   ///< Generates a new random id.
        explicit UUID(uint64_t value) : value(value) {}

        operator uint64_t() const { return value; }
        bool operator==(const UUID& other) const { return value == other.value; }

    private:
        uint64_t value;
    };

    // --- components. Plain data; behaviour lives in systems -------------------

    struct IDComponent
    {
        UUID id;
    };

    struct TagComponent
    {
        std::string tag = "Entity";
    };

    /**
     * @brief Position, rotation and scale in 3D.
     *
     * D3: one transform type, with 2D as the z=0 case. Rotation is stored as Euler
     * angles in radians because that is what a gizmo and an inspector both edit; the
     * matrix is composed on demand.
     */
    struct TransformComponent
    {
        glm::vec3 translation{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};

        glm::mat4 matrix() const
        {
            const glm::mat4 r =
                glm::rotate(glm::mat4(1.0f), rotation.x, {1, 0, 0}) *
                glm::rotate(glm::mat4(1.0f), rotation.y, {0, 1, 0}) *
                glm::rotate(glm::mat4(1.0f), rotation.z, {0, 0, 1});
            return glm::translate(glm::mat4(1.0f), translation) * r *
                   glm::scale(glm::mat4(1.0f), scale);
        }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 color{1.0f};
        ScrapGameEngine::Texture2D* texture = nullptr;
        std::string texturePath;      ///< Serialized; the pointer is resolved on load.
    };

    enum class ProjectionKind { Orthographic, Perspective };

    struct CameraComponent
    {
        ProjectionKind projection = ProjectionKind::Orthographic;
        float orthoSize = 5.0f;
        float fovDegrees = 60.0f;
        float nearClip = -1.0f;
        float farClip = 1000.0f;
        bool primary = true;          ///< Which camera the runtime renders through.
    };

    /** @brief Which built-in primitive a MeshRendererComponent draws. */
    enum class PrimitiveKind { Cube, Sphere, Plane, Custom };

    /**
     * @brief Lit 3D geometry.
     *
     * The mesh pointer is resolved at load from `primitive` (or, later, from a glTF
     * path), for the same reason SpriteRenderer serializes a path and not an address.
     */
    struct MeshRendererComponent
    {
        PrimitiveKind primitive = PrimitiveKind::Cube;
        std::shared_ptr<ScrapGameEngine::Mesh3D> mesh;
        std::string meshPath;

        glm::vec4 albedo{0.8f, 0.8f, 0.82f, 1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float emissive = 0.0f;
    };

    enum class LightKind { Directional, Point };

    /** @brief A light. Direction comes from the entity's transform rotation. */
    struct LightComponent
    {
        LightKind kind = LightKind::Point;
        glm::vec3 color{1.0f};
        float intensity = 1.0f;
        float range = 10.0f;
    };

    /**
     * @brief Names a managed type to instantiate for this entity.
     *
     * The C# runtime is already hosted; this is the hook that gives it something to
     * attach to. Resolution happens when play mode starts.
     */
    struct ScriptComponent
    {
        std::string typeName;         ///< e.g. "Game.PlayerController, GameScripts"
    };
}
