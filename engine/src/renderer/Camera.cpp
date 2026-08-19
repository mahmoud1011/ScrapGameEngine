#include "renderer/Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ScrapGameEngine
{
    glm::mat4 Camera::projection{1.0f};
    glm::mat4 Camera::view{1.0f};
    glm::mat4 Camera::vp{1.0f};
    glm::vec3 Camera::position{0.0f};
    float Camera::aspectRatio = 1.0f;
    int Camera::viewportWidth = 1;
    int Camera::viewportHeight = 1;
    CameraConfig Camera::config{};
    bool Camera::isDirty = true;

    void Camera::init(CameraConfig cfg, int width, int height)
    {
        config = cfg;
        recalculate(width, height);
    }

    void Camera::recalculate(int width, int height)
    {
        if (width <= 0 || height <= 0) return;

        viewportWidth = width;
        viewportHeight = height;
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        rebuildProjection();
    }

    void Camera::rebuildProjection()
    {
        if (config.projectionType == ProjectionType::Perspective)
        {
            projection = glm::perspective(glm::radians(config.fovDegrees), aspectRatio,
                                          config.perspectiveNear, config.perspectiveFar);
        }
        else
        {
            const float maxY = config.orthoSize;
            const float maxX = maxY * aspectRatio;
            projection = glm::ortho(-maxX, maxX, -maxY, maxY, config.orthoNear, config.orthoFar);
        }
        isDirty = true;
    }

    void Camera::rebuildViewIfDirty()
    {
        if (!isDirty) return;

        view = glm::inverse(glm::translate(glm::mat4(1.0f), position));
        vp = projection * view;
        isDirty = false;
    }

    float Camera::getAspectRatio() { return aspectRatio; }
    float Camera::getOrthoSize() { return config.orthoSize; }
    ProjectionType Camera::getProjectionType() { return config.projectionType; }
    glm::vec3 Camera::getPosition() { return position; }

    void Camera::setProjectionType(ProjectionType type)
    {
        if (config.projectionType == type) return;
        config.projectionType = type;
        rebuildProjection();
    }

    void Camera::setOrthoSize(float size)
    {
        config.orthoSize = size;
        if (config.projectionType == ProjectionType::Orthographic) rebuildProjection();
    }

    void Camera::setFieldOfView(float degrees)
    {
        config.fovDegrees = degrees;
        if (config.projectionType == ProjectionType::Perspective) rebuildProjection();
    }

    glm::mat4 Camera::getMatrix_projection() { return projection; }

    glm::mat4 Camera::getMatrix_view()
    {
        rebuildViewIfDirty();
        return view;
    }

    glm::mat4 Camera::getMatrix_viewProjection()
    {
        rebuildViewIfDirty();
        return vp;
    }

    void Camera::translate(glm::vec3 translation)
    {
        position += translation;
        isDirty = true;
    }

    void Camera::translate(float x, float y, float z)
    {
        translate(glm::vec3(x, y, z));
    }

    void Camera::setPosition(glm::vec3 value)
    {
        position = value;
        isDirty = true;
    }

    void Camera::setPosition(float x, float y, float z)
    {
        setPosition(glm::vec3(x, y, z));
    }

    glm::vec3 Camera::screenToWorld(glm::vec2 screenPos)
    {
        rebuildViewIfDirty();

        // Screen origin is top-left, NDC origin is centre with +Y up, hence the flip.
        const float normX = 2.0f * (screenPos.x / static_cast<float>(viewportWidth)) - 1.0f;
        const float normY = -(2.0f * (screenPos.y / static_cast<float>(viewportHeight)) - 1.0f);

        const glm::vec4 ndc(normX, normY, 0.0f, 1.0f);
        glm::vec4 world = glm::inverse(vp) * ndc;

        // Perspective divide is a no-op under ortho (w stays 1) but required otherwise.
        if (world.w != 0.0f) world /= world.w;

        return glm::vec3(world.x, world.y, world.z);
    }
}
