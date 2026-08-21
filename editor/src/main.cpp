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
#include "ProjectHub.h"
#include "ScrapTheme.h"

#include "platform/AppWindow.h"
#include "platform/AppIcon.h"
#include "platform/Input.h"
#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "renderer/Renderer2D.h"
#include "renderer/Mesh3D.h"
#include "renderer/Renderer3D.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "scene/SceneSerializer.h"
#include "project/Project.h"
#include "assets/GltfImporter.h"
#include "rhi/Framebuffer.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/DotNetHost.h"
#include "scripting/ScriptEngine.h"
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <ImGuizmo.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <cmath>
#include <cstdlib>
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

        // A lit 3D set alongside the sprites, so the editor opens on something that
        // exercises both renderers in one frame.
        struct Solid { const char* name; Scrap::PrimitiveKind kind; glm::vec3 pos;
                       glm::vec3 scale; glm::vec4 albedo; float metallic; float roughness; };
        const Solid solids[] = {
            {"Floor",  Scrap::PrimitiveKind::Plane,  {0.0f, -1.8f, 0.0f}, {2.2f, 1.0f, 2.2f},
             {0.20f, 0.22f, 0.25f, 1.0f}, 0.0f, 0.85f},
            {"Brass Sphere", Scrap::PrimitiveKind::Sphere, {-1.5f, -0.9f, -1.0f}, {1.4f, 1.4f, 1.4f},
             {0.85f, 0.62f, 0.28f, 1.0f}, 1.0f, 0.28f},
            {"Steel Cube",   Scrap::PrimitiveKind::Cube,   {1.3f, -1.1f, -0.6f}, {1.2f, 1.2f, 1.2f},
             {0.62f, 0.66f, 0.70f, 1.0f}, 1.0f, 0.42f},
            {"Patina Cube",  Scrap::PrimitiveKind::Cube,   {0.1f, -1.25f, 1.1f}, {0.9f, 0.9f, 0.9f},
             {0.18f, 0.64f, 0.59f, 1.0f}, 0.0f, 0.55f},
        };
        for (const auto& solid : solids)
        {
            Scrap::Entity e = scene->createEntity(solid.name);
            auto& t = e.getComponent<Scrap::TransformComponent>();
            t.translation = solid.pos;
            t.scale = solid.scale;
            auto& mr = e.addComponent<Scrap::MeshRendererComponent>();
            mr.primitive = solid.kind;
            mr.albedo = solid.albedo;
            mr.metallic = solid.metallic;
            mr.roughness = solid.roughness;
        }

        Scrap::Entity sun = scene->createEntity("Sun");
        auto& sunLight = sun.addComponent<Scrap::LightComponent>();
        sunLight.kind = Scrap::LightKind::Directional;
        sunLight.intensity = 3.2f;
        sunLight.color = {1.0f, 0.96f, 0.90f};
        sun.getComponent<Scrap::TransformComponent>().rotation = {-0.85f, 0.6f, 0.0f};

        Scrap::Entity fill = scene->createEntity("Fill Light");
        auto& fillLight = fill.addComponent<Scrap::LightComponent>();
        fillLight.color = {0.35f, 0.75f, 0.95f};
        fillLight.intensity = 14.0f;
        fillLight.range = 12.0f;
        fill.getComponent<Scrap::TransformComponent>().translation = {2.4f, 1.4f, 2.2f};

        // Scripted out of the box, so pressing Play shows the managed loop running.
        scene->findByTag("Marker").addComponent<Scrap::ScriptComponent>().typeName = "Game.Spinner";
        scene->findByTag("Player").addComponent<Scrap::ScriptComponent>().typeName = "Game.Pulser";
        scene->findByTag("Brass Sphere").addComponent<Scrap::ScriptComponent>().typeName = "Game.Spinner";
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

    // --- prefabs -----------------------------------------------------------
    Scrap::Entity source = reloaded->findByTag("Brass Sphere");
    if (!source) { std::cerr << "FAIL: prefab source missing\n"; return 1; }
    source.getComponent<Scrap::TransformComponent>().translation = {7.0f, 1.5f, -2.0f};

    const std::string prefabPath = "selftest.scrapprefab";
    if (!Scrap::SceneSerializer::savePrefab(source, prefabPath))
    {
        std::cerr << "FAIL: savePrefab\n"; return 1;
    }

    Scrap::Entity instanceA = Scrap::SceneSerializer::instantiatePrefab(*reloaded, prefabPath);
    Scrap::Entity instanceB = Scrap::SceneSerializer::instantiatePrefab(*reloaded, prefabPath);
    if (!instanceA || !instanceB) { std::cerr << "FAIL: instantiatePrefab\n"; return 1; }

    // Two instances must be distinct entities with distinct ids, or the second would
    // displace the first in the scene's UUID table.
    if (instanceA == instanceB || instanceA.getUUID() == instanceB.getUUID())
    {
        std::cerr << "FAIL: prefab instances share identity\n"; return 1;
    }
    if (instanceA.getUUID() == source.getUUID())
    {
        std::cerr << "FAIL: instance reused the source UUID\n"; return 1;
    }

    const glm::vec3 instancePos = instanceA.getComponent<Scrap::TransformComponent>().translation;
    if (std::abs(instancePos.x - 7.0f) > 1e-4f || std::abs(instancePos.z + 2.0f) > 1e-4f)
    {
        std::cerr << "FAIL: prefab transform not restored\n"; return 1;
    }
    if (!instanceA.hasComponent<Scrap::MeshRendererComponent>())
    {
        std::cerr << "FAIL: prefab lost MeshRenderer\n"; return 1;
    }
    if (std::abs(instanceA.getComponent<Scrap::MeshRendererComponent>().metallic - 1.0f) > 1e-4f)
    {
        std::cerr << "FAIL: prefab lost material\n"; return 1;
    }

    // --- project system ----------------------------------------------------
    {
        namespace fs = std::filesystem;
        const fs::path sandbox = fs::temp_directory_path() / "scrap-selftest-project";
        std::error_code ec;
        fs::remove_all(sandbox, ec);

        auto created = Scrap::Project::create(sandbox, "SelfTestProject");
        if (!created) { std::cerr << "FAIL: project create\n"; return 1; }

        // The standard layout must exist, or the content browser has nothing to show
        // and the editor has nowhere to put a scene.
        for (const auto& dir : {created->assetsDirectory(), created->scenesDirectory(),
                                created->scriptsDirectory(), created->prefabsDirectory()})
        {
            if (!fs::exists(dir, ec))
            {
                std::cerr << "FAIL: missing project folder " << dir.string() << "\n";
                return 1;
            }
        }
        if (!fs::exists(created->scriptsDirectory() / "Player.cs", ec))
        {
            std::cerr << "FAIL: starter script not written\n"; return 1;
        }

        // Creating over a non-empty folder must be refused rather than scattering a
        // layout through someone else's directory.
        if (Scrap::Project::create(sandbox, "Duplicate") != nullptr)
        {
            std::cerr << "FAIL: create overwrote a non-empty folder\n"; return 1;
        }

        // Reopening by folder, not just by file, since that is what a user picks.
        auto reopened = Scrap::Project::open(sandbox);
        if (!reopened) { std::cerr << "FAIL: project open by directory\n"; return 1; }
        if (reopened->getConfig().name != "SelfTestProject")
        {
            std::cerr << "FAIL: project name lost\n"; return 1;
        }
        if (reopened->getConfig().startScene != "scenes/Main.scrapscene")
        {
            std::cerr << "FAIL: start scene lost\n"; return 1;
        }

        // Paths round-trip: absolute for use, relative for storage.
        const fs::path resolved = reopened->resolve("scenes/Main.scrapscene");
        if (reopened->relativize(resolved) != "scenes/Main.scrapscene")
        {
            std::cerr << "FAIL: path relativize round trip, got "
                      << reopened->relativize(resolved) << "\n";
            return 1;
        }

        // A scene written into the project must be discoverable.
        auto projectScene = std::make_shared<Scrap::Scene>();
        projectScene->setName("Main");
        fs::create_directories(resolved.parent_path(), ec);
        if (!Scrap::SceneSerializer(projectScene).serialize(resolved.string()))
        {
            std::cerr << "FAIL: could not write scene into project\n"; return 1;
        }
        if (reopened->findScenes().size() != 1)
        {
            std::cerr << "FAIL: findScenes found " << reopened->findScenes().size() << "\n";
            return 1;
        }
        if (reopened->findScripts().size() != 1)
        {
            std::cerr << "FAIL: findScripts found " << reopened->findScripts().size() << "\n";
            return 1;
        }

        std::cout << "PROJECT OK - layout created, reopened by folder, paths round-trip, "
                  << "1 scene and 1 script discovered" << std::endl;

        fs::remove_all(sandbox, ec);
    }

    // --- glTF import -------------------------------------------------------
    for (const auto& candidate : {std::string("../assets/models/Triangle.gltf"),
                                  std::string("assets/models/Triangle.gltf")})
    {
        if (!std::filesystem::exists(candidate)) continue;

        auto primitives = GltfImporter::load(candidate);
        if (primitives.empty()) { std::cerr << "FAIL: gltf produced no primitives\n"; return 1; }

        const auto& first = primitives.front();
        // CPU-side only: this test runs without a GL context, which is precisely why
        // parsing and upload are separate.
        if (first.vertices.size() != 3)
        {
            std::cerr << "FAIL: gltf vertex count " << first.vertices.size() << "\n"; return 1;
        }
        if (first.indices.size() != 3)
        {
            std::cerr << "FAIL: gltf index count " << first.indices.size() << "\n"; return 1;
        }
        if (std::abs(first.vertices[1].position.x - 1.0f) > 1e-4f)
        {
            std::cerr << "FAIL: gltf positions wrong\n"; return 1;
        }
        if (std::abs(first.vertices[0].normal.z - 1.0f) > 1e-4f)
        {
            std::cerr << "FAIL: gltf normals wrong\n"; return 1;
        }
        // The node's translation must survive as the primitive's transform.
        if (std::abs(first.transform[3].x - 2.0f) > 1e-4f)
        {
            std::cerr << "FAIL: gltf node transform lost\n"; return 1;
        }
        if (std::abs(first.metallic - 0.25f) > 1e-4f ||
            std::abs(first.albedo.r - 0.9f) > 1e-4f)
        {
            std::cerr << "FAIL: gltf material not read\n"; return 1;
        }

        std::cout << "GLTF OK - " << primitives.size() << " primitive, "
                  << first.triangleCount() << " triangle, positions/normals/material/"
                  << "node transform all read" << std::endl;
        break;
    }

    std::cout << "PREFAB OK - two instances, distinct UUIDs, material and transform intact"
              << std::endl;

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


/**
 * Repeatable render benchmark. `ScrapEditor --stress [count]`.
 *
 * Spawns `count` meshes in a slab that extends well past the near frustum, runs a
 * fixed number of frames in each configuration, and reports mean frame time. The
 * point is that the optimizations are measured rather than asserted - and that the
 * numbers can be re-checked after any renderer change.
 */
static int runStress(int meshCount)
{
    AppWindowData windowData(1280, 720, "ScrapEngine Benchmark");
    AppWindow window(windowData);
    if (window.init(windowData) <= 0) { std::cerr << "FAIL: window\n"; return 1; }

    auto* nativeWindow = static_cast<GLFWwindow*>(window.getNativeWindow());
    // Uncapped, otherwise every configuration measures the swap interval instead.
    glfwSwapInterval(0);

    if (!Renderer::init(1280, 720)) { std::cerr << "FAIL: renderer\n"; return 1; }

    FramebufferSpec spec; spec.width = 1280; spec.height = 720; spec.depth = true;
    Framebuffer target;
    if (!target.create(spec)) { std::cerr << "FAIL: target\n"; return 1; }

    auto scene = std::make_shared<Scrap::Scene>();
    const int side = static_cast<int>(std::ceil(std::sqrt(static_cast<float>(meshCount))));
    int made = 0;
    for (int z = 0; z < side && made < meshCount; z++)
    {
        for (int x = 0; x < side && made < meshCount; x++, made++)
        {
            Scrap::Entity e = scene->createEntity("Cube");
            auto& t = e.getComponent<Scrap::TransformComponent>();
            t.translation = {static_cast<float>(x - side / 2) * 1.6f, 0.0f,
                             static_cast<float>(z) * -1.6f};
            t.scale = glm::vec3(0.7f);
            auto& mr = e.addComponent<Scrap::MeshRendererComponent>();
            mr.primitive = Scrap::PrimitiveKind::Cube;
            mr.albedo = {0.6f, 0.65f, 0.7f, 1.0f};
            mr.metallic = 0.2f;
        }
    }

    Scrap::Entity sun = scene->createEntity("Sun");
    auto& sunLight = sun.addComponent<Scrap::LightComponent>();
    sunLight.kind = Scrap::LightKind::Directional;
    sun.getComponent<Scrap::TransformComponent>().rotation = {-0.9f, 0.4f, 0.0f};

    // Fill the point-light slots, since per-frame vs per-draw uniform cost is exactly
    // what scales with them - the case the split was meant to fix.
    for (unsigned int i = 0; i < ScrapGameEngine::Renderer3D::maxPointLights(); i++)
    {
        Scrap::Entity light = scene->createEntity("Point");
        auto& lc = light.addComponent<Scrap::LightComponent>();
        lc.intensity = 6.0f;
        lc.range = 25.0f;
        light.getComponent<Scrap::TransformComponent>().translation =
            {static_cast<float>(i) * 3.0f - 10.0f, 3.0f, -6.0f};
    }

    // The camera matrices are built directly rather than through EditorCamera, which
    // reads ImGui state for its controls and there is no ImGui context here.
    const glm::vec3 cameraPosition{0.0f, 5.0f, 8.0f};
    const glm::mat4 view = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, -12.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(55.0f), 1280.0f / 720.0f,
                                                  0.1f, 300.0f);
    const glm::mat4 viewProjection = projection * view;

    constexpr int kWarmup = 30;
    constexpr int kFrames = 200;

    auto measure = [&](bool culling) {
        ScrapGameEngine::Renderer3D::setCullingEnabled(culling);
        for (int i = 0; i < kWarmup; i++)
        {
            scene->onRenderInto(target, viewProjection, cameraPosition, false);
            glfwPollEvents();
        }
        glFinish();

        const auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < kFrames; i++)
        {
            scene->onRenderInto(target, viewProjection, cameraPosition, false);
        }
        glFinish();
        const auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count() / kFrames;
    };

    // Culling stays on for both instancing runs - it is already established, and
    // leaving it on measures instancing against a realistic baseline.
    ScrapGameEngine::Renderer3D::setDepthPrepassEnabled(false);
    ScrapGameEngine::Renderer3D::setInstancingEnabled(false);
    const double withoutInstancing = measure(true);
    const auto statsNoInst = ScrapGameEngine::Renderer3D::getStats();

    ScrapGameEngine::Renderer3D::setInstancingEnabled(true);
    const double withInstancing = measure(true);
    const auto statsInst = ScrapGameEngine::Renderer3D::getStats();

    ScrapGameEngine::Renderer3D::setDepthPrepassEnabled(true);
    const double withPrepass = measure(true);
    const auto statsPrepass = ScrapGameEngine::Renderer3D::getStats();

    // Isolate the shadow pass, which renders every caster into a 2048^2 target and
    // is the obvious suspect for a frame time this far above what the triangle count
    // should cost.
    ScrapGameEngine::Renderer3D::setShadowsEnabled(false);
    const double noShadows = measure(true);
    ScrapGameEngine::Renderer3D::setShadowsEnabled(true);
    const double withShadows = measure(true);
    std::cout << "shadows OFF     " << noShadows << " ms/frame\n";
    std::cout << "shadows ON      " << withShadows << " ms/frame\n";

    const double withoutCulling = measure(false);
    const auto statsOff = ScrapGameEngine::Renderer3D::getStats();
    const double withCulling = measure(true);
    const auto statsOn = ScrapGameEngine::Renderer3D::getStats();

    std::cout << "\n=== ScrapEngine render benchmark ===\n"
              << "meshes submitted   " << meshCount << "\n"
              << "point lights       " << ScrapGameEngine::Renderer3D::maxPointLights() << "\n"
              << "frames per config  " << kFrames << "\n\n";

    std::cout << "culling OFF   " << withoutCulling << " ms/frame   drawn "
              << statsOff.meshCount << "   culled " << statsOff.culled
              << "   uniform sets " << statsOff.uniformUploads << "\n";
    std::cout << "culling ON    " << withCulling << " ms/frame   drawn "
              << statsOn.meshCount << "   culled " << statsOn.culled
              << "   uniform sets " << statsOn.uniformUploads << "\n";

    if (withCulling > 0.0)
    {
        std::cout << "\nculling speedup    " << (withoutCulling / withCulling) << "x\n";
    }

    std::cout << "\ninstancing OFF  " << withoutInstancing << " ms/frame   draw calls "
              << statsNoInst.drawCalls << "\n";
    std::cout << "instancing ON   " << withInstancing << " ms/frame   draw calls "
              << statsInst.drawCalls << "   batches " << statsInst.batches
              << "   instances " << statsInst.instanced << "\n";
    if (withInstancing > 0.0)
    {
        std::cout << "instancing speedup " << (withoutInstancing / withInstancing) << "x\n";
    }

    std::cout << "\n+ depth prepass " << withPrepass << " ms/frame   prepass draws "
              << statsPrepass.prepassDraws << "\n";
    if (withPrepass > 0.0)
    {
        std::cout << "overall speedup    " << (withoutInstancing / withPrepass)
                  << "x vs one draw call per mesh\n";
    }

    // Material now rides in the instance data rather than in uniforms, so the
    // per-draw uniform cost is gone entirely rather than merely reduced.
    const unsigned int perDrawBefore =
        statsOn.meshCount * (6 + 6 + ScrapGameEngine::Renderer3D::maxPointLights() * 4);
    std::cout << "\nper-draw uniform sets " << perDrawBefore
              << " originally -> " << statsOn.uniformUploads
              << " now (material moved into instance data)\n" << std::endl;

    Renderer::shutdown();
    return 0;
}

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        const std::string arg = argv[i];
        if (arg == "--selftest") return runSelfTest();
#ifdef SCRAP_HAS_DOTNET
        if (arg == "--scripttest") return runScriptTest();
#endif
        if (arg == "--stress")
        {
            const int count = (i + 1 < argc) ? std::atoi(argv[i + 1]) : 2000;
            return runStress(count > 0 ? count : 2000);
        }
    }

    if (!glfwInit())
    {
        std::cerr << "[EDITOR] GLFW failed to initialise." << std::endl;
        return 1;
    }

    auto project = Scrap::Project::active();
    if (!project) project = ProjectHub::run();
    if (!project)
    {
        // Hub closed without choosing - a normal exit, not a failure.
        glfwTerminate();
        return 0;
    }

    const std::string title = "ScrapEditor - " + project->getConfig().name;
    AppWindowData windowData(1600, 900, title);
    AppWindow window(windowData);

    if (window.init(windowData) <= 0)
    {
        std::cerr << "[EDITOR] window creation failed." << std::endl;
        return 1;
    }

    auto* nativeWindow = static_cast<GLFWwindow*>(window.getNativeWindow());
    AppIcon::setFromPng(nativeWindow, AppIcon::locateBrandingIcon());

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

    // Scene target carries the entity-id attachment for picking; the game target does
    // not, since nothing picks in it and the extra clear would be wasted every frame.
    FramebufferSpec sceneSpec; sceneSpec.width = 1280; sceneSpec.height = 720;
    sceneSpec.depth = true;  sceneSpec.entityId = true;
    FramebufferSpec gameSpec = sceneSpec; gameSpec.entityId = false;
    if (!ctx.sceneTarget.create(sceneSpec) || !ctx.gameTarget.create(gameSpec))
    {
        std::cerr << "[EDITOR] could not create render targets." << std::endl;
        return 1;
    }
    ctx.logInfo("ScrapEditor ready.");

    namespace fs = std::filesystem;
    Panels::setContentRoot(project->assetsDirectory());
    ctx.logInfo("Project: " + project->getConfig().name);

    ctx.editorScene = std::make_shared<Scrap::Scene>();
    ctx.activeScene = ctx.editorScene;
    ctx.scenePath = project->resolve(project->getConfig().startScene).string();

    {
        Scrap::SceneSerializer loader(ctx.editorScene);
        if (loader.deserialize(ctx.scenePath))
        {
            ctx.logInfo("Loaded scene: " + project->relativize(ctx.scenePath));
        }
        else
        {
            // A new project has no scene yet, so one is built and written where the
            // project config already says the start scene lives.
            ctx.editorScene->setName("Main");
            createStarterScene(ctx.editorScene);
            fs::create_directories(fs::path(ctx.scenePath).parent_path());
            Scrap::SceneSerializer(ctx.editorScene).serialize(ctx.scenePath);
            ctx.logInfo("Created starter scene at " +
                        project->relativize(ctx.scenePath));
        }
    }

    // Open in the projection that suits the content: a scene with meshes wants a
    // perspective view, a pure sprite scene does not.
    if (ctx.editorScene->raw().view<Scrap::MeshRendererComponent>().size() > 0)
    {
        ctx.camera.setPerspective(true);
        ctx.camera.setZoom(9.0f);
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

        // Two passes: the Scene view through the editor camera, and the Game view
        // through the scene's own camera. Both run in edit mode too, so the Game view
        // previews framing while you author - which is the point of having it.
        ctx.activeScene->onRenderInto(ctx.sceneTarget, ctx.camera.getViewProjection(),
                                      ctx.camera.getPosition(), ctx.showGrid);
        ctx.gameHasCamera = ctx.activeScene->onRenderRuntimeInto(ctx.gameTarget);

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
                        ? "Saved " + project->relativize(ctx.scenePath)
                        : "Could not save " + ctx.scenePath);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Close Project"))
                {
                    // Returning to the hub means tearing this window down; simplest
                    // and least surprising is to exit and let the user relaunch.
                    running = false;
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
        Panels::drawGameView();
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
