#include "Renderer.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h> // GLFW header for window management and context creation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::rotate, glm::scale
#include "Texture2D.h" 
#include "Camera.h"
#include <iostream>
using namespace ScrapGameEngine;

std::vector<DrawCommand> Renderer::draws;
bool Renderer::isRendering = false;
glm::mat4 Renderer::vpMatrix;

void Renderer::init()
{
    glEnable(GL_TEXTURE_2D); // Enable texturing
    glEnable(GL_BLEND); // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending
}

void Renderer::submitCommand(DrawCommand dc)
{
    // Ensure that draw commands are only submitted during the rendering phase
    if (!isRendering)
    {
        std::cerr << "Error: Rendering commands can only be submitted during the render step!" << std::endl;
        return;
    }

    // Apply transformations here
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), dc.translation);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(dc.rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, dc.scale);
    dc.modelMatrix = modelMatrix;

    // Add the draw command to the list of commands to be processed later
    draws.push_back(dc);
}

void Renderer::beginFrame()
{
    isRendering = true;
    clear(); // Clear the renderer

    // Set the vp (view-projection) matrix to identity initially
    Renderer::vpMatrix = Camera::getMatrix_viewProjection();
}

void Renderer::endFrame()
{
    // Set common settings
    glPushMatrix(); // Push matrix to stack.
    glLoadMatrixf(&vpMatrix[0][0]); // Load the view-projection matrix.

    glEnableClientState(GL_VERTEX_ARRAY); // Enable vertex array state.
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); // Enable texture coordinate array state.

    // Draw all render requests
    for (DrawCommand dc : draws)
    {
        glPushMatrix(); // Push matrix for each object

        // Apply model transformations
        glMultMatrixf(&dc.modelMatrix[0][0]);

        // Apply materials
        glColor4fv(&dc.tint[0]); // Set the tint color

        // Bind the texture if it has a valid textureID
        if (dc.textureID != 0)
        {
            glBindTexture(GL_TEXTURE_2D, dc.textureID);
        }

        // Draw object using the vertex and texture arrays
        glBindBuffer(GL_ARRAY_BUFFER, dc.meshId);
        glVertexPointer(3, GL_FLOAT, dc.vertexStride, 0); // Vertex positions
        glTexCoordPointer(2, GL_FLOAT, dc.vertexStride, (void*)12); // Texture coordinates (offset by 12 bytes)

        // Draw the triangles
        glDrawArrays(GL_TRIANGLES, 0, dc.vertexCount);

        // Unbind the texture after drawing
        if (dc.textureID != 0)
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glPopMatrix(); // Pop matrix for this object
    }

    // Unset common settings
    glDisableClientState(GL_VERTEX_ARRAY); // Disable vertex array state
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); // Disable texture coordinate array state
    glPopMatrix(); // Pop the view-projection matrix

    // Clear the draws and reset rendering state
    draws.clear();
    isRendering = false;
}

int Renderer::load()
{
    // Initialize GLAD
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialized successfully." << std::endl;
        return 1; // Return 1 if successful
    }
    else
    {
        std::cout << "Failed to initialize GLAD." << std::endl;
        return 0; // Return 0 if failed
    }
}

void Renderer::setViewport(int x, int y, int width, int height)
{
    // Set the viewport parameters using the OpenGL function
    glViewport(x, y, width, height);
    Camera::recalculate(width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
    // Set the clear color using the OpenGL function
    glClearColor(r, g, b, a);
}

void Renderer::clear()
{
    // Clear the framebuffer using the OpenGL function
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color and depth buffers
}

