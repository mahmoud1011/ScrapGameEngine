#include <glad/glad.h>
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "platform/Input.h"
#include "core/Time.h"
#include "platform/AppWindow.h"
#include "scene/SceneStateMachine.h"
#include <iostream>
#include "renderer/TextureAllocator.h"
#include "renderer/MeshAllocator.h"
#include "audio/AudioDevice.h"
#include "core/Application.h"

using namespace ScrapGameEngine;

Application* Application::instance = nullptr;
int Application::init()
{
    // windowData already carries the size and title passed to the constructor.
    int result = window.init(windowData);

    if (result > 0)
    {
        targetFrameRate = 0;
        frameTime = 0.0f;
        isRunning = true;

        window.setWindowEventCallback([this](AppWindowEventType eventType, void* payload)
            {
                processWindowEvent(eventType, payload);
            });

        instance = this;
    }

    return result;
}

void Application::processWindowEvent(AppWindowEventType eventType, void* payload)
{
    switch (eventType)
    {
        case AppWindowEventType::CLOSE:
        {
            isRunning = false;
            break;
        }
        case AppWindowEventType::FRAMEBUFFER_RESIZE:
        {
            break;
        }
    }
}

void Application::run()
{
    Renderer::setClearColor(0.25, 0.25, 0.25, 1.0);
    Renderer::init();

    CameraConfig cfg;
    Camera::init(cfg, windowData.width, windowData.height);

    // Late Init
    Input::init(&window);

    while (isRunning)
    {
        // Timing --------------------------------------------------------------
        Time::processTime(frameTime);
        float deltaTime = Time::getDeltaTime();

        // Input Processing ----------------------------------------------------
        Input::process();
        if (Input::getKey(KeyCode::ESCAPE))
        {
            isRunning = false;
        }

        // Updates -------------------------------------------------------------
        SceneStateMachine::update(deltaTime);

        // Rendering -----------------------------------------------------------
        Renderer::clear();
        Renderer::beginFrame();        
        SceneStateMachine::render();
        Renderer::endFrame();

        // Finalize ------------------------------------------------------------
        window.update();
    }

    // TODO:: should have one allocater to rule them all
    MeshAllocator::releaseUnusedMeshes();
    TextureAllocator::releaseUnusedTextures();

    SceneStateMachine::dispose();

    cleanup();
}

void Application::cleanup()
{
    // Release the shared output device. The window is cleaned up by AppWindow itself.
    AudioDevice::shutdown();
}

void Application::setTargetFrameRate(unsigned int frameRate)
{
    instance->targetFrameRate = frameRate;

    if (frameRate > 0) {
        // Cap the frame rate
        instance->frameTime = 1.0f / static_cast<float>(frameRate);
    }
    else if (frameRate == -1) {
        // Uncapped frame rate (no limit)
        instance->frameTime = 0.0f;  // Could be interpreted as no delay
    }
    else {
        // Handle invalid frame rates (e.g., negative values other than -1)
        std::cerr << "[FRAMEWORK] Invalid frame rate value." << std::endl;
        instance->frameTime = 0.0f;  // Default to no frame delay
    }

    std::cout << "[FRAMEWORK] Setting target frame rate to: " << frameRate << std::endl; // Print the frameRate
}

void Application::quit()
{
    instance->isRunning = false;
}