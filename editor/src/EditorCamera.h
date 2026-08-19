#pragma once

#include <glm/glm.hpp>

namespace Scrap::Editor
{
    /**
     * @class EditorCamera
     * @brief The camera the editor views the scene through.
     *
     * Deliberately owned by the editor and entirely separate from the engine's camera:
     * flying around a scene must not disturb what the game's camera sees. This is the
     * concrete reason the engine's static Camera has to become a component.
     *
     * Controls follow the convention most DCC tools share, so muscle memory carries:
     *   - middle drag, or alt+middle: pan
     *   - right drag: orbit (perspective) / pan (orthographic)
     *   - wheel: zoom, toward the cursor in 2D
     */
    class EditorCamera
    {
    public:
        EditorCamera() = default;

        /** @brief Recomputes projection for a new viewport size. */
        void setViewportSize(float width, float height);

        /**
         * @brief Applies input for this frame.
         * @param deltaTime Seconds since the last frame.
         * @param hasFocus False when the viewport is not hovered, so the camera ignores
         *        input that belongs to another panel.
         */
        void update(float deltaTime, bool hasFocus);

        const glm::mat4& getView() const { return view; }
        const glm::mat4& getProjection() const { return projection; }
        glm::mat4 getViewProjection() const { return projection * view; }

        glm::vec3 getPosition() const { return position; }
        void setPosition(const glm::vec3& value) { position = value; dirty = true; }

        bool isPerspective() const { return perspective; }
        void setPerspective(bool value) { perspective = value; dirty = true; }

        float getZoom() const { return zoom; }
        void setZoom(float value);

        /** @brief Frames the origin at a sensible default distance. */
        void reset();

        /** @brief Converts a viewport-local point to a world position on the z=0 plane. */
        glm::vec2 viewportToWorld2D(const glm::vec2& viewportPos) const;

    private:
        void recalculate();

        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};

        glm::vec3 position{0.0f, 0.0f, 5.0f};
        glm::vec3 focalPoint{0.0f};

        float pitch = 0.0f;
        float yaw = 0.0f;

        float zoom = 5.0f;          ///< Ortho half-height, and orbit distance in 3D.
        float fovDegrees = 50.0f;
        float nearClip = 0.03f;
        float farClip = 1000.0f;

        float viewportWidth = 1.0f;
        float viewportHeight = 1.0f;

        bool perspective = false;   ///< 2D by default; the toolbar toggles it.
        bool dirty = true;

        glm::vec2 lastMouse{0.0f};
        bool hasLastMouse = false;
    };
}
