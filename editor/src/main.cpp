/**
 * ScrapEditor
 *
 * Drives the engine's subsystems directly rather than going through Application,
 * which is still shaped around the sandbox game's flow. The editor owning its own
 * loop is also what the engine needs long term: Application becomes one host among
 * several rather than the only entry point.
 */
#include "EditorContext.h"
#include "ImGuiLayer.h"
#include "Panels.h"
#include "ScrapTheme.h"

#include "platform/AppWindow.h"
#include "platform/Input.h"
#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "renderer/Renderer2D.h"
#include "scene/GameObject.h"
#include "scene/GameObjectCollection.h"
#include "renderer/SpriteRenderer.h"
#include "scene/Transform.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/DotNetHost.h"
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <iostream>

using namespace ScrapGameEngine;
using namespace Scrap::Editor;

namespace
{
    /**
     * Builds a small scene so the editor opens onto something rather than a void.
     * Replaced by real scene loading once serialization exists.
     */
    void createStarterScene()
    {
        auto& ctx = EditorContext::get();

        struct Placement { const char* name; float x, y, scale; glm::vec3 color; };
        const Placement placements[] = {
            {"Ground",     0.0f, -1.4f, 1.0f, {0.22f, 0.26f, 0.30f}},
            {"Player",    -1.2f,  0.0f, 1.0f, {0.18f, 0.64f, 0.59f}},
            {"Crate A",    0.6f,  0.2f, 1.0f, {0.55f, 0.42f, 0.28f}},
            {"Crate B",    1.4f, -0.4f, 1.0f, {0.48f, 0.36f, 0.24f}},
            {"Marker",     0.0f,  1.1f, 1.0f, {0.83f, 0.64f, 0.24f}},
        };

        for (const auto& p : placements)
        {
            GameObject* go = GameObject::Create(p.name);
            go->transform->setPosition({p.x, p.y});
            go->transform->setScale(p.name == std::string("Ground")
                                        ? glm::vec2{6.0f, 0.4f}
                                        : glm::vec2{0.7f, 0.7f});

            auto* sprite = go->addComponent<SpriteRenderer>();
            sprite->setColour(p.color);
        }

        ctx.logInfo("Starter scene created with 5 objects.");
    }
}

int main()
{
    AppWindowData windowData(1600, 900, "ScrapEditor");
    AppWindow window(windowData);

    if (window.init(windowData) <= 0)
    {
        std::cerr << "[EDITOR] window creation failed." << std::endl;
        return 1;
    }

    auto* nativeWindow = static_cast<GLFWwindow*>(window.getNativeWindow());

    bool running = true;
    window.setWindowEventCallback([&running](AppWindowEventType type, void*) {
        if (type == AppWindowEventType::CLOSE) running = false;
    });

    if (!Renderer::init(1600, 900))
    {
        std::cerr << "[EDITOR] renderer failed to initialise." << std::endl;
        return 1;
    }
    Renderer::setClearColor(0.086f, 0.106f, 0.125f, 1.0f);   // Palette::Ground

    Input::init(&window);

    if (!ImGuiLayer::init(nativeWindow))
    {
        std::cerr << "[EDITOR] ImGui failed to initialise." << std::endl;
        return 1;
    }

    auto& ctx = EditorContext::get();
    ctx.camera.reset();
    ctx.logInfo("ScrapEditor ready.");

    // Root the content browser at the sandbox's assets if they are alongside us.
    namespace fs = std::filesystem;
    for (const auto& candidate : {fs::path("../assets"), fs::path("assets"), fs::path(".")})
    {
        if (fs::exists(candidate)) { Panels::setContentRoot(candidate); break; }
    }

    createStarterScene();

#ifdef SCRAP_HAS_DOTNET
    // C# scripting. Signatures must match the [UnmanagedCallersOnly] declarations in
    // ScrapScript.Bootstrap exactly - the runtime hands back a raw function pointer
    // and does not check.
    using InitializeFn = int (*)(int);
    using TickFn = void (*)(float);
    using AverageFrameTimeFn = float (*)();

    TickFn scriptTick = nullptr;
    AverageFrameTimeFn scriptAverage = nullptr;

    if (DotNetHost::initialize("scripting/ScrapScript.runtimeconfig.json",
                               "scripting/ScrapScript.dll"))
    {
        const char* type = "Scrap.Bootstrap, ScrapScript";
        if (auto* init = reinterpret_cast<InitializeFn>(DotNetHost::getFunction(type, "Initialize")))
        {
            init(SCRAP_ENGINE_VERSION);
            ctx.logInfo("C# scripting online (CoreCLR).");
        }
        scriptTick = reinterpret_cast<TickFn>(DotNetHost::getFunction(type, "Tick"));
        scriptAverage = reinterpret_cast<AverageFrameTimeFn>(
            DotNetHost::getFunction(type, "AverageFrameTime"));
    }
    else
    {
        ctx.logWarning("C# scripting unavailable - running native only.");
    }
#endif

    auto previous = std::chrono::high_resolution_clock::now();

    while (running && !glfwWindowShouldClose(nativeWindow))
    {
        const auto now = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(now - previous).count();
        previous = now;

        ctx.frameMs = deltaTime * 1000.0f;
        ctx.fps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;

        glfwPollEvents();
        Input::process();

        // Objects queued last frame get folded in, and play mode ticks the scene.
        GameObjectCollection::update(ctx.isPlaying() ? deltaTime : 0.0f);
        ctx.camera.update(deltaTime, ctx.viewportHovered);

#ifdef SCRAP_HAS_DOTNET
        if (scriptTick && ctx.isPlaying()) scriptTick(deltaTime);
#endif

        // Scene pass into the offscreen target the viewport panel samples.
        Renderer::beginFrameWith(ctx.camera.getViewProjection());
        GameObjectCollection::render();
        Renderer::endFrameOffscreen();

        int displayWidth = 0, displayHeight = 0;
        glfwGetFramebufferSize(nativeWindow, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.043f, 0.055f, 0.067f, 1.0f);          // Palette::Void
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiLayer::begin();
        ImGuizmo::BeginFrame();

        ImGuiLayer::beginDockspace();
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("New Scene", "Ctrl+N", false, false);
                ImGui::MenuItem("Open Scene...", "Ctrl+O", false, false);
                ImGui::MenuItem("Save Scene", "Ctrl+S", false, false);
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) running = false;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
                ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Grid", nullptr, &ctx.showGrid);
                if (ImGui::MenuItem("Reset Camera")) ctx.camera.reset();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        Panels::drawToolbar();
        Panels::drawViewport();
        Panels::drawHierarchy();
        Panels::drawInspector();
        Panels::drawContentBrowser();
        Panels::drawConsole();
        Panels::drawStats();

        ImGuiLayer::endDockspace();

        // Gizmo hotkeys, only when a text field is not eating them.
        if (!ImGuiLayer::wantsKeyboard())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_W)) ctx.gizmoOp = GizmoOp::Translate;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) ctx.gizmoOp = GizmoOp::Rotate;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) ctx.gizmoOp = GizmoOp::Scale;
            if (ImGui::IsKeyPressed(ImGuiKey_Q)) ctx.gizmoOp = GizmoOp::None;
            if (ImGui::IsKeyPressed(ImGuiKey_F)) ctx.camera.reset();
        }

        ImGuiLayer::end(displayWidth, displayHeight);
        glfwSwapBuffers(nativeWindow);
    }

#ifdef SCRAP_HAS_DOTNET
    if (scriptAverage)
    {
        std::cout << "[DOTNET] managed average frame time: "
                  << scriptAverage() * 1000.0f << " ms" << std::endl;
    }
    DotNetHost::shutdown();
#endif

    ImGuiLayer::shutdown();
    GameObjectCollection::dispose();
    Renderer::shutdown();
    return 0;
}
