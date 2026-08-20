#include "scene/SceneSerializer.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>

namespace YAML
{
    template<> struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& v)
        {
            Node n; n.push_back(v.x); n.push_back(v.y); n.push_back(v.z);
            n.SetStyle(EmitterStyle::Flow);
            return n;
        }
        static bool decode(const Node& n, glm::vec3& v)
        {
            if (!n.IsSequence() || n.size() != 3) return false;
            v = {n[0].as<float>(), n[1].as<float>(), n[2].as<float>()};
            return true;
        }
    };

    template<> struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& v)
        {
            Node n; n.push_back(v.x); n.push_back(v.y); n.push_back(v.z); n.push_back(v.w);
            n.SetStyle(EmitterStyle::Flow);
            return n;
        }
        static bool decode(const Node& n, glm::vec4& v)
        {
            if (!n.IsSequence() || n.size() != 4) return false;
            v = {n[0].as<float>(), n[1].as<float>(), n[2].as<float>(), n[3].as<float>()};
            return true;
        }
    };
}

namespace Scrap
{
    namespace
    {
        YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
        {
            out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
            return out;
        }

        YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
        {
            out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
            return out;
        }

        void serializeEntity(YAML::Emitter& out, Entity entity)
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Entity" << YAML::Value << static_cast<uint64_t>(entity.getUUID());

            if (auto* tag = entity.tryGetComponent<TagComponent>())
            {
                out << YAML::Key << "Tag" << YAML::Value << tag->tag;
            }

            if (auto* t = entity.tryGetComponent<TransformComponent>())
            {
                out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Translation" << YAML::Value << t->translation;
                out << YAML::Key << "Rotation" << YAML::Value << t->rotation;
                out << YAML::Key << "Scale" << YAML::Value << t->scale;
                out << YAML::EndMap;
            }

            if (auto* s = entity.tryGetComponent<SpriteRendererComponent>())
            {
                out << YAML::Key << "SpriteRenderer" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Color" << YAML::Value << s->color;
                // The pointer is a runtime resolution of this path, so only the path
                // is written - a texture address means nothing in a file.
                out << YAML::Key << "Texture" << YAML::Value << s->texturePath;
                out << YAML::EndMap;
            }

            if (auto* c = entity.tryGetComponent<CameraComponent>())
            {
                out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Projection" << YAML::Value
                    << (c->projection == ProjectionKind::Perspective ? "Perspective" : "Orthographic");
                out << YAML::Key << "OrthoSize" << YAML::Value << c->orthoSize;
                out << YAML::Key << "Fov" << YAML::Value << c->fovDegrees;
                out << YAML::Key << "Near" << YAML::Value << c->nearClip;
                out << YAML::Key << "Far" << YAML::Value << c->farClip;
                out << YAML::Key << "Primary" << YAML::Value << c->primary;
                out << YAML::EndMap;
            }

            if (auto* script = entity.tryGetComponent<ScriptComponent>())
            {
                out << YAML::Key << "Script" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Type" << YAML::Value << script->typeName;
                out << YAML::EndMap;
            }

            out << YAML::EndMap;
        }
    }

    bool SceneSerializer::serialize(const std::string& path)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << scene->getName();
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        // Reverse so the file order matches creation order; EnTT iterates newest first.
        auto& registry = scene->raw();
        std::vector<entt::entity> entities;
        for (auto e : registry.view<IDComponent>()) entities.push_back(e);
        for (auto it = entities.rbegin(); it != entities.rend(); ++it)
        {
            serializeEntity(out, Entity{*it, scene.get()});
        }

        out << YAML::EndSeq << YAML::EndMap;

        std::ofstream file(path);
        if (!file)
        {
            std::cerr << "[SCENE] could not open " << path << " for writing." << std::endl;
            return false;
        }
        file << out.c_str();
        return true;
    }

    bool SceneSerializer::deserialize(const std::string& path)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path);
        }
        catch (const YAML::Exception& e)
        {
            std::cerr << "[SCENE] could not parse " << path << ": " << e.what() << std::endl;
            return false;
        }

        if (!data["Scene"])
        {
            std::cerr << "[SCENE] " << path << " is not a scene file." << std::endl;
            return false;
        }

        scene->setName(data["Scene"].as<std::string>());
        scene->raw().clear();
        scene->byUUID.clear();

        const auto entities = data["Entities"];
        if (!entities) return true;   // a valid, empty scene

        for (const auto& node : entities)
        {
            const auto uuid = node["Entity"].as<uint64_t>();
            const std::string tag = node["Tag"] ? node["Tag"].as<std::string>() : "Entity";

            Entity entity = scene->createEntityWithUUID(UUID(uuid), tag);

            if (const auto t = node["Transform"])
            {
                auto& transform = entity.getComponent<TransformComponent>();
                transform.translation = t["Translation"].as<glm::vec3>();
                transform.rotation = t["Rotation"].as<glm::vec3>();
                transform.scale = t["Scale"].as<glm::vec3>();
            }

            if (const auto s = node["SpriteRenderer"])
            {
                auto& sprite = entity.addComponent<SpriteRendererComponent>();
                sprite.color = s["Color"].as<glm::vec4>();
                if (s["Texture"]) sprite.texturePath = s["Texture"].as<std::string>();
            }

            if (const auto c = node["Camera"])
            {
                auto& camera = entity.addComponent<CameraComponent>();
                camera.projection = c["Projection"].as<std::string>() == "Perspective"
                                        ? ProjectionKind::Perspective
                                        : ProjectionKind::Orthographic;
                camera.orthoSize = c["OrthoSize"].as<float>();
                camera.fovDegrees = c["Fov"].as<float>();
                camera.nearClip = c["Near"].as<float>();
                camera.farClip = c["Far"].as<float>();
                camera.primary = c["Primary"].as<bool>();
            }

            if (const auto script = node["Script"])
            {
                entity.addComponent<ScriptComponent>().typeName = script["Type"].as<std::string>();
            }
        }

        return true;
    }
}
