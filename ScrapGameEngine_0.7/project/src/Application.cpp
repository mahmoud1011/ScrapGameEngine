#include <glad/glad.h>
#include "Renderer.h"
#include "Camera.h"
#include "Input.h"
#include "Time.h"
#include "AppWindow.h"
#include "SceneStateMachine.h"
#include "MainMenuScene.h"
#include <iostream>
#include "TextureAllocator.h"
#include "MeshAllocater.h"
#include "Application.h"

using namespace ScrapGameEngine;

// Application variables ========
float x = 0;
float y = 0;
bool isRunning = true;

float currentTime = 0.0f;
float prevTime = 0.0f;
float deltaTime = 0.0f;

unsigned int targetFrameRate = 0;
float frameTime = 0.0f;

Application* Application::instance = nullptr;
// ===============================
int Application::init()
{
    windowData = AppWindowData(600, 600, "Scrap Game Engine");
    int result = window.init(windowData); 

    if (result > 0)
    {
        cleanup(); // Cleanup after running

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
        // Camera update
        SceneStateMachine::update(deltaTime);

        float y = 0;
        Camera::setPosition(0, y, 0);
        glm::vec2 screenPos(300, 150);
        auto worldPos = Camera::screenToWorld(screenPos);


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
}

void Application::cleanup()
{
    // No additional cleanup needed, the window will be automatically cleaned up.
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