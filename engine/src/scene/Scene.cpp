#include "scene/Scene.h"
#include "scene/Entity.h"

#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "renderer/Mesh3D.h"
#include "renderer/Renderer2D.h"
#include "renderer/Renderer3D.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/ScriptEngine.h"
#endif

#include <random>

namespace Scrap
{
    namespace
    {
        std::random_device randomDevice;
        std::mt19937_64 randomEngine(randomDevice());
        std::uniform_int_distribution<uint64_t> distribution;

        /** Copies every component of one type across, matching entities by UUID. */
        template<typename T>
        void copyComponentType(entt::registry& dst, const entt::registry& src,
                               const std::unordered_map<uint64_t, entt::entity>& target)
        {
            for (auto entity : src.view<T>())
            {
                const uint64_t id = src.get<IDComponent>(entity).id;
                const auto it = target.find(id);
                if (it == target.end()) continue;
                dst.emplace_or_replace<T>(it->second, src.get<T>(entity));
            }
        }
    }

    UUID::UUID() : value(distribution(randomEngine)) {}

    Entity Scene::createEntity(const std::string& name)
    {
        return createEntityWithUUID(UUID(), name);
    }

    Entity Scene::createEntityWithUUID(UUID id, const std::string& name)
    {
        Entity entity{registry.create(), this};
        entity.addComponent<IDComponent>(id);
        entity.addComponent<TransformComponent>();
        entity.addComponent<TagComponent>(name.empty() ? "Entity" : name);
        byUUID[id] = entity;
        return entity;
    }

    void Scene::destroyEntity(Entity entity)
    {
        if (!entity.isValid()) return;
        byUUID.erase(entity.getUUID());
        registry.destroy(entity);
    }

    Entity Scene::findByUUID(UUID id)
    {
        const auto it = byUUID.find(id);
        if (it == byUUID.end()) return {};
        return Entity{it->second, this};
    }

    Entity Scene::findByTag(const std::string& tag)
    {
        for (auto entity : registry.view<TagComponent>())
        {
            if (registry.get<TagComponent>(entity).tag == tag) return Entity{entity, this};
        }
        return {};
    }

    void Scene::onUpdateRuntime(float deltaTime)
    {
#ifdef SCRAP_HAS_DOTNET
        ScriptEngine::onUpdate(deltaTime);
#else
        (void)deltaTime;
#endif
    }

    void Scene::renderSprites()
    {
        for (auto entity : registry.view<TransformComponent, SpriteRendererComponent>())
        {
            const auto& transform = registry.get<TransformComponent>(entity);
            const auto& sprite = registry.get<SpriteRendererComponent>(entity);
            ScrapGameEngine::Renderer2D::drawQuad(transform.matrix(), sprite.texture, sprite.color);
        }
    }

    void Scene::submitLights()
    {
        using namespace ScrapGameEngine;
        Renderer3D::clearPointLights();

        bool haveSun = false;
        for (auto handle : registry.view<TransformComponent, LightComponent>())
        {
            const auto& light = registry.get<LightComponent>(handle);
            const auto& transform = registry.get<TransformComponent>(handle);

            if (light.kind == LightKind::Directional)
            {
                // Direction is the entity's -Z axis, so rotating the entity aims it.
                const glm::vec3 dir = glm::normalize(
                    glm::vec3(transform.matrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
                Renderer3D::setDirectionalLight({dir, light.color, light.intensity});
                haveSun = true;
            }
            else
            {
                Renderer3D::addPointLight({transform.translation, light.color,
                                           light.intensity, light.range});
            }
        }

        // Without a sun the scene would be lit by ambient alone and read as flat, so
        // fall back to a default key light rather than showing nothing.
        if (!haveSun) Renderer3D::setDirectionalLight({});
    }

    void Scene::renderMeshes(const glm::mat4& viewProjection, const glm::vec3& cameraPosition)
    {
        using namespace ScrapGameEngine;
        auto view = registry.view<TransformComponent, MeshRendererComponent>();
        if (view.begin() == view.end()) return;

        submitLights();
        Renderer3D::beginScene(viewProjection, cameraPosition);

        for (auto handle : view)
        {
            auto& mesh = registry.get<MeshRendererComponent>(handle);
            const auto& transform = registry.get<TransformComponent>(handle);

            // Primitives are built lazily so a deserialized scene needs no extra pass.
            if (!mesh.mesh)
            {
                switch (mesh.primitive)
                {
                    case PrimitiveKind::Sphere: mesh.mesh = Mesh3D::createSphere(); break;
                    case PrimitiveKind::Plane:  mesh.mesh = Mesh3D::createPlane(4.0f); break;
                    case PrimitiveKind::Cube:
                    case PrimitiveKind::Custom: mesh.mesh = Mesh3D::createCube(); break;
                }
            }

            Renderer3D::drawMesh(mesh.mesh, transform.matrix(),
                                 {mesh.albedo, mesh.metallic, mesh.roughness, mesh.emissive});
        }

        Renderer3D::endScene();
    }

    void Scene::onRenderEditor(const glm::mat4& viewProjection, const glm::vec3& cameraPosition,
                               bool drawGrid)
    {
        ScrapGameEngine::Renderer::beginFrameWith(viewProjection);

        // 3D first so depth is laid down, then sprites blend over it.
        renderMeshes(viewProjection, cameraPosition);
        if (drawGrid) ScrapGameEngine::Renderer3D::drawGrid(viewProjection);
        renderSprites();

        ScrapGameEngine::Renderer::endFrameOffscreen();
    }

    void Scene::onRenderRuntime()
    {
        // The scene renders through its own primary camera, which is exactly what the
        // static Camera could not express.
        glm::mat4 viewProjection{1.0f};
        bool found = false;

        for (auto entity : registry.view<TransformComponent, CameraComponent>())
        {
            const auto& camera = registry.get<CameraComponent>(entity);
            if (!camera.primary) continue;

            const auto& transform = registry.get<TransformComponent>(entity);
            const float aspect = ScrapGameEngine::Camera::getAspectRatio();

            const glm::mat4 projection =
                camera.projection == ProjectionKind::Perspective
                    ? glm::perspective(glm::radians(camera.fovDegrees), aspect,
                                       0.03f, camera.farClip)
                    : glm::ortho(-camera.orthoSize * aspect, camera.orthoSize * aspect,
                                 -camera.orthoSize, camera.orthoSize,
                                 camera.nearClip, camera.farClip);

            viewProjection = projection * glm::inverse(transform.matrix());
            found = true;
            break;
        }

        if (!found) return;   // nothing to see through

        const glm::vec3 cameraPosition =
            glm::vec3(glm::inverse(viewProjection)[3]);

        ScrapGameEngine::Renderer::beginFrameWith(viewProjection);
        renderMeshes(viewProjection, cameraPosition);
        renderSprites();
        ScrapGameEngine::Renderer::endFrameOffscreen();
    }

    std::shared_ptr<Scene> Scene::copy(const std::shared_ptr<Scene>& source)
    {
        auto target = std::make_shared<Scene>();
        target->name = source->name;

        // Recreate every entity under its original UUID first, so component copies can
        // resolve references by id rather than by a recycled entt handle.
        for (auto entity : source->registry.view<IDComponent>())
        {
            const UUID id = source->registry.get<IDComponent>(entity).id;
            const auto& tag = source->registry.get<TagComponent>(entity).tag;
            target->createEntityWithUUID(id, tag);
        }

        copyComponentType<TransformComponent>(target->registry, source->registry, target->byUUID);
        copyComponentType<SpriteRendererComponent>(target->registry, source->registry, target->byUUID);
        copyComponentType<CameraComponent>(target->registry, source->registry, target->byUUID);
        copyComponentType<MeshRendererComponent>(target->registry, source->registry, target->byUUID);
        copyComponentType<LightComponent>(target->registry, source->registry, target->byUUID);
        copyComponentType<ScriptComponent>(target->registry, source->registry, target->byUUID);

        return target;
    }
}
