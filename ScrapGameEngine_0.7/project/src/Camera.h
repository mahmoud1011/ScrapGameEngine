#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace ScrapGameEngine
{
    /**
     * @struct CameraConfig
     * @brief Configuration structure for the Camera.
     *
     * This struct contains parameters that define the camera's behavior such as
     * orthographic size. It allows for easy customization of camera settings.
     */
    struct CameraConfig
    {
        float orthoSize = 1.0f;  /**< Default orthographic size is 1.0 */
    };

    /**
     * @class Camera
     * @brief A Camera class that handles 3D transformations and projection.
     *
     * The Camera class provides functionalities for setting the camera's position,
     * calculating projection matrices, translating the camera, and converting screen coordinates to world coordinates.
     */
    class Camera
    {
    public:
        /**
         * @brief Default constructor is deleted.
         *
         * The Camera cannot be instantiated directly since it is intended to be used as a singleton or
         * as a static utility class. Initialize it with the `init` method instead.
         */
        Camera() = delete;

        /**
         * @brief Initializes the camera with a configuration, width, and height.
         * @param cfg Configuration for the camera.
         * @param width The width of the viewport.
         * @param height The height of the viewport.
         */
        static void init(CameraConfig cfg, int width, int height);

        /**
         * @brief Recalculates projection matrices based on new width and height.
         * @param width The new width of the viewport.
         * @param height The new height of the viewport.
         */
        static void recalculate(int width, int height);

        /**
         * @brief Gets the aspect ratio of the camera.
         * @return The aspect ratio as a float.
         */
        static float getAspectRatio();

        /**
         * @brief Gets the orthographic size of the camera.
         * @return The orthographic size.
         */
        float getOrthoSize() const;

        /**
         * @brief Gets the projection matrix.
         * @return The projection matrix.
         */
        static glm::mat4 getMatrix_projection();

        /**
         * @brief Gets the view matrix.
         * @return The view matrix.
         */
        static glm::mat4 getMatrix_view();

        /**
         * @brief Gets the view-projection matrix.
         * @return The view-projection matrix.
         */
        static glm::mat4 getMatrix_viewProjection();

        /**
         * @brief Translates the camera by a specified vector.
         * @param translation The translation vector.
         */
        static void translate(glm::vec3 translation);

        /**
         * @brief Translates the camera by specified x, y, z values.
         * @param x The x translation.
         * @param y The y translation.
         * @param z The z translation.
         */
        static void translate(float x, float y, float z);

        /**
         * @brief Sets the position of the camera.
         * @param position The new position vector.
         */
        static void setPosition(glm::vec3 position);

        /**
         * @brief Sets the position of the camera using x, y, z coordinates.
         * @param x The x coordinate.
         * @param y The y coordinate.
         * @param z The z coordinate.
         */
        static void setPosition(float x, float y, float z);

        /**
         * @brief Converts screen coordinates to world coordinates.
         * @param screenPos The screen position as a 2D vector.
         * @return The corresponding world position as a 3D vector.
         */
        static glm::vec3 screenToWorld(glm::vec2 screenPos);

    private:
        static glm::mat4 projection;       /**< Projection matrix. */
        static glm::mat4 view;             /**< View matrix. */
        static glm::mat4 vp;               /**< View-projection matrix. */
        static glm::vec3 position;         /**< Position of the camera. */
        static float aspectRatio;          /**< Aspect ratio of the camera. */
        static CameraConfig config;        /**< Camera configuration settings. */
        static bool isDirty;                /**< Flag indicating if matrices need to be recalculated. */
    };
}