#include "EditorCamera.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>

namespace Scrap::Editor
{
    namespace
    {
        constexpr float kMinZoom = 0.05f;
        constexpr float kMaxZoom = 500.0f;
        constexpr float kOrbitSpeed = 0.006f;
        constexpr float kPanSpeed = 0.0018f;
    }

    void EditorCamera::setViewportSize(float width, float height)
    {
        if (width <= 0.0f || height <= 0.0f) return;
        if (width == viewportWidth && height == viewportHeight) return;

        viewportWidth = width;
        viewportHeight = height;
        dirty = true;
    }

    void EditorCamera::setZoom(float value)
    {
        zoom = std::clamp(value, kMinZoom, kMaxZoom);
        dirty = true;
    }

    void EditorCamera::reset()
    {
        position = {0.0f, 0.0f, 5.0f};
        focalPoint = {0.0f, 0.0f, 0.0f};
        pitch = yaw = 0.0f;
        zoom = 5.0f;
        dirty = true;
    }

    void EditorCamera::update(float deltaTime, bool hasFocus)
    {
        (void)deltaTime;

        const ImGuiIO& io = ImGui::GetIO();
        const glm::vec2 mouse{io.MousePos.x, io.MousePos.y};

        if (!hasFocus)
        {
            // Drop the anchor so the next drag does not jump by however far the cursor
            // travelled while the viewport was unfocused.
            hasLastMouse = false;
            if (dirty) recalculate();
            return;
        }

        const glm::vec2 delta = hasLastMouse ? (mouse - lastMouse) : glm::vec2{0.0f};
        lastMouse = mouse;
        hasLastMouse = true;

        const bool alt = io.KeyAlt;
        const bool middle = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
        const bool right = ImGui::IsMouseDown(ImGuiMouseButton_Right);

        if (middle || (alt && right))
        {
            // Pan. Scaled by zoom so a drag moves the same number of screen pixels
            // regardless of how far in you are.
            const float scale = perspective ? zoom * kPanSpeed : (zoom * 2.0f / viewportHeight);
            const glm::vec3 rightAxis = glm::vec3(glm::inverse(view)[0]);
            const glm::vec3 upAxis = glm::vec3(glm::inverse(view)[1]);

            focalPoint += -rightAxis * delta.x * scale;
            focalPoint += upAxis * delta.y * scale;
            dirty = true;
        }
        else if (right && perspective)
        {
            yaw += delta.x * kOrbitSpeed;
            pitch = std::clamp(pitch + delta.y * kOrbitSpeed, -1.55f, 1.55f);
            dirty = true;
        }

        if (io.MouseWheel != 0.0f)
        {
            // Exponential so each notch is a constant proportion, not a constant amount.
            setZoom(zoom * std::pow(0.88f, io.MouseWheel));
        }

        if (dirty) recalculate();
    }

    void EditorCamera::recalculate()
    {
        const float aspect = viewportWidth / viewportHeight;

        if (perspective)
        {
            projection = glm::perspective(glm::radians(fovDegrees), aspect, nearClip, farClip);

            const glm::quat orientation = glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
            position = focalPoint + (orientation * glm::vec3(0.0f, 0.0f, 1.0f)) * zoom;
            view = glm::inverse(glm::translate(glm::mat4(1.0f), position) *
                                glm::mat4_cast(orientation));
        }
        else
        {
            const float halfH = zoom;
            const float halfW = halfH * aspect;
            projection = glm::ortho(-halfW, halfW, -halfH, halfH, -1000.0f, 1000.0f);

            position = {focalPoint.x, focalPoint.y, zoom};
            view = glm::inverse(glm::translate(glm::mat4(1.0f),
                                               glm::vec3(focalPoint.x, focalPoint.y, 0.0f)));
        }

        dirty = false;
    }

    glm::vec2 EditorCamera::viewportToWorld2D(const glm::vec2& viewportPos) const
    {
        // Viewport origin is top-left; NDC is centred with +Y up.
        const float ndcX = 2.0f * (viewportPos.x / viewportWidth) - 1.0f;
        const float ndcY = -(2.0f * (viewportPos.y / viewportHeight) - 1.0f);

        glm::vec4 world = glm::inverse(projection * view) * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
        if (world.w != 0.0f) world /= world.w;

        return {world.x, world.y};
    }
}
