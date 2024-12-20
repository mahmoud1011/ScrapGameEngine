#include "AppWindow.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
using namespace ScrapGameEngine;

ScrapGameEngine::AppWindow::AppWindow(const AppWindowData& data) : windowData(data), nativeWindow(nullptr)
{   }

AppWindow::~AppWindow()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void* AppWindow::getNativeWindow()
{
	return nativeWindow;
}

// return 0 if fail, 1 if success
int AppWindow::init(AppWindowData data)
{
    // try init GLFW
    if (!glfwInit())
    {
        std::cout << "[FRAMEWORK] Failed to initialize GLFW" << std::endl;
        return 0;
    }

    // cache the window data
    windowData = data;

    // set window context to OpenGL 2.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // try create the window
    GLFWwindow* window = glfwCreateWindow(
        windowData.width, windowData.height,
        windowData.title.c_str(), nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "[FRAMEWORK] Failed to create Window" << std::endl;
        return 0;
    }

    // set the window context as the current context
    glfwMakeContextCurrent(window);

    // try initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "[FRAMEWORK] Failed to initialize GLAD" << std::endl;
        return 0;
    }

    // Store window data pointer in the user pointer
    glfwSetWindowUserPointer(window, &windowData);

    // Set window close callback
    glfwSetWindowCloseCallback(window, [](GLFWwindow* win)
        {
            std::cout << "[FRAMEWORK] Window close callback..." << std::endl;

            AppWindowData* winData = static_cast<AppWindowData*>(glfwGetWindowUserPointer(win));
            winData->func_cb(AppWindowEventType::CLOSE, 0);

            // Tell GLFW to actually close the window
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        });


    // Set framebuffer size callback to handle window resize
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height)
        {
            AppWindowData* winData = static_cast<AppWindowData*>(glfwGetWindowUserPointer(win));
            winData->width = width;
            winData->height = height;
            winData->func_cb(AppWindowEventType::FRAMEBUFFER_RESIZE, 0);

            // Update the OpenGL viewport
            glViewport(0, 0, width, height);

            std::cout << "[FRAMEWORK] Window resized callback: " << width << "x" << height << std::endl; // Print new size to console
        });

    // Window created successfully, cache the reference
    nativeWindow = window;

    // Return success
    return 1;
}

void AppWindow::update()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void AppWindow::setWindowEventCallback(AppWindowEventCallbackFn fn)
{
	windowData.func_cb = fn;
}

void AppWindow::setSize(int width, int height)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeWindow);
	glfwSetWindowSize(window, width, height);
	windowData.width = width;
	windowData.height = height;
}

// Returns the screen size as glm::vec2 (width, height)
glm::vec2 AppWindow::getScreenSize() const
{
	return glm::vec2(windowData.width, windowData.height);
}
