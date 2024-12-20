#pragma once
#include <glm/glm.hpp> // Include GLM for vector and matrix types
#include <glm/mat4x4.hpp>
#include <vector>

namespace ScrapGameEngine
{
    /**
     * @struct DrawCommand
     * @brief Represents a single draw command for rendering a mesh.
     *
     * This structure contains all the information necessary to render a mesh,
     * including its transformation, color, texture, and more.
     */
    struct DrawCommand
    {
        unsigned int meshId;         /**< ID of the mesh to draw. */
        unsigned int vertexStride;   /**< Size of each vertex in bytes. */
        unsigned int vertexCount;    /**< Number of vertices to draw. */
        glm::vec4 tint;              /**< Tint color for the mesh (RGBA). */
        glm::vec3 translation;       /**< Position to translate the mesh. */
        float rotationZ;             /**< Rotation angle around Z-axis in degrees. */
        glm::vec3 scale;             /**< Scale factors (x, y, z). */
        unsigned int textureID;      /**< ID of the texture to use. */
        glm::mat4 modelMatrix;       /**< Model transformation matrix. */
    };

    /**
     * @class Renderer
     * @brief A static class responsible for handling rendering operations.
     *
     * The Renderer class provides methods for initializing the rendering system,
     * submitting draw commands, and managing frame rendering. It operates as a
     * static utility to ensure consistent rendering throughout the application.
     */
    class Renderer
    {
    private:
        Renderer() = delete; /**< Deleted constructor to enforce static usage. */

        static std::vector<DrawCommand> draws; /**< Collection of draw commands to process. */
        static bool isRendering;               /**< Flag indicating whether rendering is active. */
        static glm::mat4 vpMatrix;             /**< View-projection matrix for rendering. */

    public:
        /**
         * @brief Initializes the Renderer system.
         *
         * Sets up necessary resources for rendering operations.
         */
        static void init();

        /**
         * @brief Submits a draw command to the Renderer.
         * @param dc The draw command to submit.
         *
         * The draw command will be processed during the current frame rendering.
         */
        static void submitCommand(DrawCommand dc);

        /**
         * @brief Begins a new frame for rendering.
         *
         * Prepares the Renderer for processing draw commands and rendering a frame.
         */
        static void beginFrame();

        /**
         * @brief Ends the current frame and executes all submitted draw commands.
         *
         * Finalizes the frame and sends all draw commands to the GPU for rendering.
         */
        static void endFrame();

        /**
         * @brief Loads necessary resources or configurations.
         * @return An integer indicating the result of the load operation.
         */
        static int load();

        /**
         * @brief Sets the viewport dimensions for rendering.
         * @param x The x-coordinate of the viewport origin.
         * @param y The y-coordinate of the viewport origin.
         * @param width The width of the viewport in pixels.
         * @param height The height of the viewport in pixels.
         */
        static void setViewport(int x, int y, int width, int height);

        /**
         * @brief Sets the clear color for the Renderer.
         * @param r The red component (0.0 to 1.0).
         * @param g The green component (0.0 to 1.0).
         * @param b The blue component (0.0 to 1.0).
         * @param a The alpha component (0.0 to 1.0).
         */
        static void setClearColor(float r, float g, float b, float a);

        /**
         * @brief Clears the rendering buffers.
         *
         * Clears the screen using the set clear color, preparing it for the next frame.
         */
        static void clear();
    };
}
