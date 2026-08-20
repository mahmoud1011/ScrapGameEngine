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
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "scene/SceneSerializer.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/DotNetHost.h"
#include "scripting/ScriptEngine.h"
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <cmath>
#include <iostream>

using namespace ScrapGameEngine;
using namespace Scrap::Editor;

namespace
{
    /**
     * Builds a small scene so the editor opens onto something rather than a void.
     * Used only when no scene file is present.
     */
    void createStarterScene(const std::shared_ptr<Scrap::Scene>& scene)
    {
        struct Placement { const char* name; glm::vec3 pos; glm::vec3 scale; glm::vec4 color; };
        const Placement placements[] = {
            {"Ground",  {0.0f, -1.4f, 0.0f}, {6.0f, 0.4f, 1.0f}, {0.22f, 0.26f, 0.30f, 1.0f}},
            {"Player",  {-1.2f, 0.0f, 0.0f}, {0.7f, 0.7f, 1.0f}, {0.18f, 0.64f, 0.59f, 1.0f}},
            {"Crate A", {0.6f,  0.2f, 0.0f}, {0.7f, 0.7f, 1.0f}, {0.55f, 0.42f, 0.28f, 1.0f}},
            {"Crate B", {1.4f, -0.4f, 0.0f}, {0.7f, 0.7f, 1.0f}, {0.48f, 0.36f, 0.24f, 1.0f}},
            {"Marker",  {0.0f,  1.1f, 0.0f}, {0.7f, 0.7f, 1.0f}, {0.83f, 0.64f, 0.24f, 1.0f}},
        };

        for (const auto& p : placements)
        {
            Scrap::Entity e = scene->createEntity(p.name);
            auto& t = e.getComponent<Scrap::TransformComponent>();
            t.translation = p.pos;
            t.scale = p.scale;
            e.addComponent<Scrap::SpriteRendererComponent>().color = p.color;
        }

        // A camera so play mode has something to render through.
        Scrap::Entity camera = scene->createEntity("Main Camera");
        camera.addComponent<Scrap::CameraComponent>();

        // Scripted out of the box, so pressing Play shows the managed loop running.
        scene->findByTag("Marker").addComponent<Scrap::ScriptComponent>().typeName = "Game.Spinner";
        scene->findByTag("Player").addComponent<Scrap::ScriptComponent>().typeName = "Game.Pulser";
    }
}

/**
 * Headless check of the scene round trip: build, save, reload, copy, and compare.
 * Run as `ScrapEditor --selftest`. Cheap to run in CI later, and it exercises the
 * serializer without needing anyone to click through the UI.
 */
static int runSelfTest()
{
    auto scene = std::make_shared<Scrap::Scene>();
    scene->setName("SelfTest");
    createStarterScene(scene);

    const size_t originalCount = scene->entityCount();
    Scrap::Entity player = scene->findByTag("Player");
    if (!player) { std::cerr << "FAIL: Player not found\n"; return 1; }

    const Scrap::UUID playerId = player.getUUID();
    player.getComponent<Scrap::TransformComponent>().translation = {3.5f, -2.25f, 0.75f};

    const std::string path = "selftest.scrapscene";
    if (!Scrap::SceneSerializer(scene).serialize(path)) { std::cerr << "FAIL: serialize\n"; return 1; }

    auto reloaded = std::make_shared<Scrap::Scene>();
    if (!Scrap::SceneSerializer(reloaded).deserialize(path)) { std::cerr << "FAIL: deserialize\n"; return 1; }

    if (reloaded->entityCount() != originalCount)
    {
        std::cerr << "FAIL: entity count " << reloaded->entityCount()
                  << " != " << originalCount << "\n";
        return 1;
    }
    if (reloaded->getName() != "SelfTest") { std::cerr << "FAIL: scene name lost\n"; return 1; }

    Scrap::Entity restored = reloaded->findByUUID(playerId);
    if (!restored) { std::cerr << "FAIL: UUID did not survive the round trip\n"; return 1; }

    const glm::vec3 t = restored.getComponent<Scrap::TransformComponent>().translation;
    if (std::abs(t.x - 3.5f) > 1e-4f || std::abs(t.y + 2.25f) > 1e-4f || std::abs(t.z - 0.75f) > 1e-4f)
    {
        std::cerr << "FAIL: translation " << t.x << "," << t.y << "," << t.z << "\n";
        return 1;
    }
    if (!restored.hasComponent<Scrap::SpriteRendererComponent>())
    {
        std::cerr << "FAIL: SpriteRenderer lost\n"; return 1;
    }
    if (!reloaded->findByTag("Main Camera").hasComponent<Scrap::CameraComponent>())
    {
        std::cerr << "FAIL: Camera lost\n"; return 1;
    }

    // Copy-on-play must be a genuine deep copy: mutating the snapshot must not touch
    // the original, which is what makes Stop a pointer swap rather than a reload.
    auto snapshot = Scrap::Scene::copy(reloaded);
    if (snapshot->entityCount() != originalCount) { std::cerr << "FAIL: copy count\n"; return 1; }
    snapshot->findByUUID(playerId).getComponent<Scrap::TransformComponent>().translation = {99.0f, 99.0f, 99.0f};
    if (reloaded->findByUUID(playerId).getComponent<Scrap::TransformComponent>().translation.x != 3.5f)
    {
        std::cerr << "FAIL: copy aliased the source scene\n"; return 1;
    }

    std::cout << "SELFTEST OK - " << originalCount
              << " entities, UUIDs stable, transforms exact, copy is deep." << std::endl;
    return 0;
}


#ifdef SCRAP_HAS_DOTNET
/**
 * Headless check of the scripting loop, end to end: host the runtime, bind the script
 * engine, instantiate a managed script onto an entity, tick it, and confirm the engine
 * side actually changed. Run as `ScrapEditor --scripttest`.
 *
 * This is the check that matters for the interop boundary - a script that runs but
 * cannot move anything would still print happily.
 */
static int runScriptTest()
{
    if (!ScrapGameEngine::DotNetHost::initialize("scripting/ScrapScript.runtimeconfig.json",
                                                 "scripting/ScrapScript.dll"))
    {
        std::cerr << "FAIL: runtime did not host\n";
        return 1;
    }
    if (!Scrap::ScriptEngine::initialize()) { std::cerr << "FAIL: script engine\n"; return 1; }

    auto scene = std::make_shared<Scrap::Scene>();
    Scrap::Entity spinner = scene->createEntity("Spinner");
    spinner.getComponent<Scrap::TransformComponent>().translation = {1.0f, 2.0f, 0.0f};
    spinner.addComponent<Scrap::ScriptComponent>().typeName = "Game.Spinner";

    Scrap::Entity pulser = scene->createEntity("Pulser");
    pulser.addComponent<Scrap::ScriptComponent>().typeName = "Game.Pulser";

    // A type that does not exist must fail cleanly rather than take the process down.
    Scrap::Entity broken = scene->createEntity("Broken");
    broken.addComponent<Scrap::ScriptComponent>().typeName = "Game.DoesNotExist";

    Scrap::ScriptEngine::onRuntimeStart(scene.get());
    if (Scrap::ScriptEngine::liveInstanceCount() != 2)
    {
        std::cerr << "FAIL: expected 2 live instances, got "
                  << Scrap::ScriptEngine::liveInstanceCount() << "\n";
        return 1;
    }

    const float startRotation = spinner.getComponent<Scrap::TransformComponent>().rotation.z;
    const glm::vec3 startScale = pulser.getComponent<Scrap::TransformComponent>().scale;

    for (int frame = 0; frame < 30; frame++) Scrap::ScriptEngine::onUpdate(1.0f / 60.0f);

    const auto& spinnerT = spinner.getComponent<Scrap::TransformComponent>();
    const auto& pulserT = pulser.getComponent<Scrap::TransformComponent>();

    // Managed -> native writes.
    if (std::abs(spinnerT.rotation.z - startRotation) < 1e-3f)
    {
        std::cerr << "FAIL: script did not rotate the entity\n";
        return 1;
    }
    if (std::abs(spinnerT.translation.y - 2.0f) < 1e-4f)
    {
        std::cerr << "FAIL: script did not bob the entity\n";
        return 1;
    }
    // Native -> managed reads: Spinner captured its origin in OnCreate, so X must be
    // preserved exactly rather than reset to zero.
    if (std::abs(spinnerT.translation.x - 1.0f) > 1e-4f)
    {
        std::cerr << "FAIL: script lost the original position, x=" << spinnerT.translation.x << "\n";
        return 1;
    }
    if (std::abs(pulserT.scale.x - startScale.x) < 1e-4f)
    {
        std::cerr << "FAIL: pulser did not change scale\n";
        return 1;
    }

    Scrap::ScriptEngine::onRuntimeStop();
    if (Scrap::ScriptEngine::liveInstanceCount() != 0)
    {
        std::cerr << "FAIL: instances survived stop\n";
        return 1;
    }

    std::cout << "SCRIPTTEST OK - 2 instances ran, 1 bad type rejected, "
              << "rotation " << startRotation << " -> " << spinnerT.rotation.z
              << ", origin preserved, stop released all." << std::endl;

    Scrap::ScriptEngine::shutdown();
    ScrapGameEngine::DotNetHost::shutdown();
    return 0;
}
#endif

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        const std::string arg = argv[i];
        if (arg == "--selftest") return runSelfTest();
#ifdef SCRAP_HAS_DOTNET
        if (arg == "--scripttest") return runScriptTest();
#endif
    }

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

    ctx.editorScene = std::make_shared<Scrap::Scene>();
    ctx.activeScene = ctx.editorScene;
    ctx.scenePath = "assets/scenes/Sample.scrapscene";

    {
        Scrap::SceneSerializer loader(ctx.editorScene);
        if (loader.deserialize(ctx.scenePath))
        {
            ctx.logInfo("Loaded scene: " + ctx.scenePath);
        }
        else
        {
            ctx.editorScene->setName("Sample");
            createStarterScene(ctx.editorScene);
            ctx.logInfo("No scene file found - created a starter scene.");
        }
    }

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

        if (Scrap::ScriptEngine::initialize()) ctx.logInfo("Script engine bound.");
        else ctx.logWarning("Script engine unavailable - ScriptComponents will not run.");
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

        if (ctx.isPlaying()) ctx.activeScene->onUpdateRuntime(deltaTime);
        ctx.camera.update(deltaTime, ctx.viewportHovered);

#ifdef SCRAP_HAS_DOTNET
        if (scriptTick && ctx.isPlaying()) scriptTick(deltaTime);
#endif

        // Scene pass into the offscreen target the viewport panel samples. In play
        // mode the scene renders through its own primary camera instead of the
        // editor's - the whole point of CameraComponent.
        if (ctx.isPlaying()) ctx.activeScene->onRenderRuntime();
        else                 ctx.activeScene->onRenderEditor(ctx.camera.getViewProjection());

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
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    ctx.onStop();
                    ctx.editorScene = std::make_shared<Scrap::Scene>();
                    ctx.activeScene = ctx.editorScene;
                    ctx.clearSelection();
                    ctx.logInfo("New scene.");
                }
                if (ImGui::MenuItem("Reload Scene", "Ctrl+O"))
                {
                    ctx.onStop();
                    Scrap::SceneSerializer loader(ctx.editorScene);
                    ctx.logInfo(loader.deserialize(ctx.scenePath)
                        ? "Reloaded " + ctx.scenePath
                        : "Could not reload " + ctx.scenePath);
                    ctx.clearSelection();
                }
                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                {
                    std::filesystem::create_directories(
                        std::filesystem::path(ctx.scenePath).parent_path());
                    Scrap::SceneSerializer saver(ctx.editorScene);
                    ctx.logInfo(saver.serialize(ctx.scenePath)
                        ? "Saved " + ctx.scenePath
                        : "Could not save " + ctx.scenePath);
                }
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

            if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
            {
                std::filesystem::create_directories(
                    std::filesystem::path(ctx.scenePath).parent_path());
                Scrap::SceneSerializer saver(ctx.editorScene);
                ctx.logInfo(saver.serialize(ctx.scenePath)
                    ? "Saved " + ctx.scenePath : "Could not save " + ctx.scenePath);
            }
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
    Scrap::ScriptEngine::shutdown();
    DotNetHost::shutdown();
#endif

    ImGuiLayer::shutdown();
    Renderer::shutdown();
    return 0;
}
