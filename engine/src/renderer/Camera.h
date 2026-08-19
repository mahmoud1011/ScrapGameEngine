#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace ScrapGameEngine
{
    /**
     * @enum ProjectionType
     * @brief Whether the camera projects orthographically or with perspective.
     */
    enum class ProjectionType
    {
        Orthographic,
        Perspective
    };

    /**
     * @struct CameraConfig
     * @brief Projection settings.
     *
     * Carries both projections rather than only ortho, so 3D content needs no change
     * to the camera contract when Renderer3D lands - only a different projectionType.
     */
    struct CameraConfig
    {
        ProjectionType projectionType = ProjectionType::Orthographic;

        float orthoSize = 1.0f;        /**< Half-height of the ortho view volume. */
        float orthoNear = -1.0f;
        float orthoFar = 1.0f;

        float fovDegrees = 60.0f;      /**< Vertical field of view, perspective only. */
        float perspectiveNear = 0.1f;
        float perspectiveFar = 1000.0f;
    };

    /**
     * @class Camera
     * @brief The scene camera.
     *
     * Still static, which is what blocks an editor from holding its own camera
     * alongside the game's. Phase 2 replaces this with a CameraComponent plus a
     * separate EditorCamera; the projection maths here moves across intact.
     */
    class Camera
    {
    public:
        Camera() = delete;

        static void init(CameraConfig cfg, int width, int height);
        static void recalculate(int width, int height);

        static float getAspectRatio();
        static float getOrthoSize();

        static void setProjectionType(ProjectionType type);
        static ProjectionType getProjectionType();

        static void setOrthoSize(float size);
        static void setFieldOfView(float degrees);

        static glm::mat4 getMatrix_projection();
        static glm::mat4 getMatrix_view();
        static glm::mat4 getMatrix_viewProjection();

        static void translate(glm::vec3 translation);
        static void translate(float x, float y, float z);

        static void setPosition(glm::vec3 position);
        static void setPosition(float x, float y, float z);
        static glm::vec3 getPosition();

        /**
         * @brief Converts a screen-space point to world space.
         *
         * Uses the viewport the camera was last recalculated against, rather than the
         * hardcoded 600x600 the previous implementation assumed.
         */
        static glm::vec3 screenToWorld(glm::vec2 screenPos);

    private:
        static void rebuildProjection();
        static void rebuildViewIfDirty();

        static glm::mat4 projection;
        static glm::mat4 view;
        static glm::mat4 vp;
        static glm::vec3 position;
        static float aspectRatio;
        static int viewportWidth;
        static int viewportHeight;
        static CameraConfig config;
        static bool isDirty;
    };
}
