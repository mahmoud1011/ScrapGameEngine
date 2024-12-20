#pragma once
#include "Mesh.h"
#include "Texture2D.h"
#include <glm\vec4.hpp>
#include <glm\vec3.hpp>

namespace ScrapGameEngine
{
    /**
     * @struct RenderParams
     * @brief Encapsulates rendering parameters for drawing meshes.
     *
     * This structure contains material properties, transformation settings, 
     * and texture information for rendering a mesh.
     */
    struct RenderParams
    {
        glm::vec4 tint;           /**< The tint color applied to the mesh (RGBA format). */
        glm::vec3 translation;    /**< The translation vector for positioning the mesh. */
        float rotationZ;          /**< The rotation angle around the Z-axis (in radians). */
        glm::vec3 scale;          /**< The scale vector for resizing the mesh. */
        Texture2D* texture;       /**< Pointer to the texture applied to the mesh. */
    };

    /**
     * @class Graphics
     * @brief Provides static functions for rendering graphical objects.
     *
     * The Graphics class is a utility for rendering meshes using specific rendering parameters.
     * It is a static-only class and cannot be instantiated.
     */
    class Graphics
    {
    public:
        /**
         * @brief Deleted default constructor to prevent instantiation of the Graphics class.
         */
        Graphics() = delete;

        /**
         * @brief Draws a mesh with the given rendering parameters.
         * 
         * This function applies the provided transformation, material, and texture settings
         * to render the specified mesh on the screen.
         * 
         * @param _mesh Pointer to the mesh to be drawn.
         * @param params The rendering parameters to apply to the mesh.
         */
        static void drawMesh(Mesh* _mesh, RenderParams params);
    };
}
