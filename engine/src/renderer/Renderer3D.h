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
        bool castsShadows = true;
    };

    /**
     * @struct SkySettings
     * @brief A procedural gradient sky.
     *
     * Procedural rather than a cubemap: it needs no assets to ship, reads correctly at
     * any resolution, and gives the horizon an ambient tint that a flat clear colour
     * cannot. A cubemap path can sit behind the same call later.
     */
    struct SkySettings
    {
        glm::vec3 zenith{0.20f, 0.34f, 0.52f};
        glm::vec3 horizon{0.62f, 0.68f, 0.74f};
        glm::vec3 ground{0.10f, 0.11f, 0.13f};
        float sunSize = 0.03f;
        float sunIntensity = 12.0f;
        bool enabled = true;
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
        unsigned int instanced = 0;     ///< Meshes drawn as instances.
        unsigned int batches = 0;       ///< Instanced draw calls issued.
        unsigned int prepassDraws = 0;  ///< Draws issued by the depth pre-pass.
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

        /**
         * @brief Enables or disables instancing, for A/B measurement.
         *
         * With it off every mesh takes its own draw call, which is what the
         * benchmark compares against.
         */
        static void setInstancingEnabled(bool enabled);
        static bool isInstancingEnabled();

        /**
         * @brief Enables or disables the depth pre-pass.
         *
         * Worth it when the scene has heavy overdraw and an expensive fragment shader,
         * which is exactly this renderer with several point lights. Costs one extra
         * geometry pass, saves shading every hidden pixel.
         */
        static void setDepthPrepassEnabled(bool enabled);
        static bool isDepthPrepassEnabled();

        /** @brief Sets the procedural sky for subsequent passes. */
        static void setSky(const SkySettings& sky);
        static const SkySettings& getSky();

        /**
         * @brief Renders the sky behind everything drawn this pass.
         *
         * Call before geometry: it writes no depth, so anything drawn after occludes it.
         */
        static void drawSky(const glm::mat4& viewProjection);

        // --- shadows ---------------------------------------------------------

        /** @brief Enables or disables directional shadow mapping. */
        static void setShadowsEnabled(bool enabled);
        static bool areShadowsEnabled();

        /**
         * @brief Starts the depth-only pass from the sun's point of view.
         *
         * The caller draws the same meshes again between this and endShadowPass;
         * whatever is drawn is what casts. Returns false when shadows are off, so the
         * caller can skip the second traversal entirely.
         */
        static bool beginShadowPass(const glm::vec3& sceneCenter, float sceneRadius);

        /** @brief Submits a mesh to the shadow pass. Depth only, no material. */
        static void drawMeshShadow(const std::shared_ptr<Mesh3D>& mesh, const glm::mat4& transform);

        static void endShadowPass();

        /** @brief Draws the reference grid on the XZ plane. */
        static void drawGrid(const glm::mat4& viewProjection, float extent = 20.0f);

        static const Renderer3DStats& getStats();
    };
}
