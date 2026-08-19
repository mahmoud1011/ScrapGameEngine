#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "renderer/Renderer2D.h"
#include "rhi/Framebuffer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace ScrapGameEngine;

bool Renderer::isRendering = false;
glm::vec4 Renderer::clearColor{0.25f, 0.25f, 0.25f, 1.0f};

namespace
{
    Framebuffer sceneTarget;
    int windowWidth = 1;
    int windowHeight = 1;
}

bool Renderer::init(unsigned int width, unsigned int height)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    windowWidth = static_cast<int>(width);
    windowHeight = static_cast<int>(height);

    FramebufferSpec spec;
    spec.width = width;
    spec.height = height;
    spec.depth = true;
    if (!sceneTarget.create(spec))
    {
        std::cerr << "[RENDERER] could not create the scene target." << std::endl;
        return false;
    }

    if (!Renderer2D::init())
    {
        std::cerr << "[RENDERER] batcher failed to initialise." << std::endl;
        return false;
    }

    const unsigned char* version = glGetString(GL_VERSION);
    const unsigned char* renderer = glGetString(GL_RENDERER);
    std::cout << "[RENDERER] OpenGL " << (version ? reinterpret_cast<const char*>(version) : "?")
              << " on " << (renderer ? reinterpret_cast<const char*>(renderer) : "?") << std::endl;
    return true;
}

void Renderer::shutdown()
{
    Renderer2D::shutdown();
}

void Renderer::beginFrame()
{
    isRendering = true;

    sceneTarget.bind();
    clear();
    Renderer2D::beginScene(Camera::getMatrix_viewProjection());
}

void Renderer::endFrame()
{
    Renderer2D::endScene();

    // Blit the scene target onto the window. The editor will instead sample the
    // colour attachment straight into a viewport panel and skip this entirely.
    const auto& spec = sceneTarget.getSpec();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneTarget.getID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, static_cast<GLint>(spec.width), static_cast<GLint>(spec.height),
                      0, 0, windowWidth, windowHeight,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    isRendering = false;
}

void Renderer::submitCommand(const DrawCommand& dc)
{
    if (!isRendering)
    {
        std::cerr << "[RENDERER] draw submitted outside the render step - ignored." << std::endl;
        return;
    }

    Renderer2D::drawRotatedQuad(dc.translation, {dc.scale.x, dc.scale.y},
                                dc.rotationZ, dc.texture, dc.tint);
}

void Renderer::setViewport(int x, int y, int width, int height)
{
    if (width <= 0 || height <= 0) return;

    windowWidth = width;
    windowHeight = height;
    glViewport(x, y, width, height);
    sceneTarget.resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    Camera::recalculate(width, height);
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
    clearColor = {r, g, b, a};
}

void Renderer::clear()
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Framebuffer& Renderer::getSceneTarget()
{
    return sceneTarget;
}

unsigned int Renderer::getDrawCallCount()
{
    return Renderer2D::getStats().drawCalls;
}

unsigned int Renderer::getQuadCount()
{
    return Renderer2D::getStats().quadCount;
}
