#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace ScrapGameEngine
{
    class Mesh3D;

    /**
     * @struct Material
     * @brief PBR-lite surface parameters.
     *
     * Metallic-roughness rather than specular-glossiness, because it is what glTF 2.0
     * carries and what an importer will hand over unchanged.
     */
    struct Material
    {
        glm::vec4 albedo{1.0f};
        float metallic = 0.0f;
        float roughness = 0.5f;
        float emissive = 0.0f;
    };

    /**
     * @struct DirectionalLight
     * @brief A light with direction and no position, e.g. the sun.
     */
    struct DirectionalLight
    {
        glm::vec3 direction{-0.4f, -1.0f, -0.35f};
        glm::vec3 color{1.0f};
        float intensity = 3.0f;
    };

    /**
     * @struct PointLight
     * @brief A positioned light attenuating with distance.
     */
    struct PointLight
    {
        glm::vec3 position{0.0f};
        glm::vec3 color{1.0f};
        float intensity = 1.0f;
        float range = 10.0f;
    };

    struct Renderer3DStats
    {
        unsigned int drawCalls = 0;
        unsigned int meshCount = 0;
        unsigned int triangleCount = 0;
        unsigned int culled = 0;        ///< Meshes rejected by the frustum test.
        unsigned int uniformUploads = 0; ///< Per-draw uniform sets, for spotting waste.
    };

    /**
     * @struct Frustum
     * @brief Six clip planes extracted from a view-projection matrix.
     *
     * Used to reject meshes before they are submitted. A rejected mesh costs six dot
     * products; a submitted one costs a draw call, a uniform block and vertex work,
     * so this is worth doing even for modest scene sizes.
     */
    struct Frustum
    {
        glm::vec4 planes[6]{};

        /** @brief Extracts planes via the Gribb-Hartmann method. */
        static Frustum fromViewProjection(const glm::mat4& viewProjection);

        /** @brief True when a world-space sphere is at least partly inside. */
        bool intersectsSphere(const glm::vec3& center, float radius) const;
    };

    /**
     * @class Renderer3D
     * @brief Forward renderer for lit 3D geometry.
     *
     * Forward rather than deferred: correct for the handful of lights this engine will
     * carry for a long while, and it keeps transparency working without a second path.
     * The plan flags the point at which that stops being true.
     *
     * Shares the RHI and the scene framebuffer with Renderer2D, so a scene can mix 3D
     * meshes and sprites in one frame and the editor viewport shows both.
     */
    class Renderer3D
    {
    public:
        Renderer3D() = delete;

        static bool init();
        static void shutdown();

        /** @brief Starts a pass. cameraPosition is needed for specular response. */
        static void beginScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);
        static void endScene();

        /** @brief Sets the directional light for this pass. */
        static void setDirectionalLight(const DirectionalLight& light);

        /** @brief Clears accumulated point lights. Call before adding this frame's. */
        static void clearPointLights();

        /** @brief Adds a point light, up to the shader's fixed capacity. */
        static void addPointLight(const PointLight& light);

        /** @brief How many point lights the shader can hold. */
        static unsigned int maxPointLights();

        /**
         * @brief Draws a mesh with a material at a transform.
         * @param entityId Written to the id attachment for picking; -1 to skip.
         *
         * Frustum-culled against the current pass. Per-frame uniforms (lights, camera,
         * view-projection) are uploaded once in beginScene rather than per mesh.
         */
        static void drawMesh(const std::shared_ptr<Mesh3D>& mesh,
                             const glm::mat4& transform,
                             const Material& material,
                             int entityId = -1);

        /** @brief The frustum for the current pass. */
        static const Frustum& getFrustum();

        /** @brief Enables or disables frustum culling, for A/B measurement. */
        static void setCullingEnabled(bool enabled);
        static bool isCullingEnabled();

        /** @brief Draws the reference grid on the XZ plane. */
        static void drawGrid(const glm::mat4& viewProjection, float extent = 20.0f);

        static const Renderer3DStats& getStats();
    };
}
