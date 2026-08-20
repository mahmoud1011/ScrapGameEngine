#include "assets/GltfImporter.h"
#include "renderer/Mesh3D.h"

// cgltf is header-only; this is the one translation unit that compiles it.
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    /** Reads a float accessor into a vector, whatever its component type. */
    template<int N>
    std::vector<glm::vec<N, float>> readFloats(const cgltf_accessor* accessor)
    {
        std::vector<glm::vec<N, float>> out;
        if (accessor == nullptr) return out;

        out.resize(accessor->count);
        for (cgltf_size i = 0; i < accessor->count; i++)
        {
            float buffer[4] = {0, 0, 0, 0};
            // cgltf handles normalised integers and strides for us, so a mesh authored
            // with packed attributes reads the same as one with plain floats.
            if (!cgltf_accessor_read_float(accessor, i, buffer, N))
            {
                out.clear();
                return out;
            }
            for (int c = 0; c < N; c++) out[i][c] = buffer[c];
        }
        return out;
    }

    glm::mat4 worldTransform(const cgltf_node* node)
    {
        cgltf_float m[16];
        cgltf_node_transform_world(node, m);
        return glm::make_mat4(m);
    }

    /**
     * Derives per-vertex tangents from positions and UVs.
     *
     * glTF only carries tangents when the asset was exported with them, so they are
     * generated when absent - otherwise normal mapping would have nothing to work with
     * for a large share of real files.
     */
    void generateTangents(std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
    {
        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            MeshVertex& a = vertices[indices[i + 0]];
            MeshVertex& b = vertices[indices[i + 1]];
            MeshVertex& c = vertices[indices[i + 2]];

            const glm::vec3 e1 = b.position - a.position;
            const glm::vec3 e2 = c.position - a.position;
            const glm::vec2 d1 = b.uv - a.uv;
            const glm::vec2 d2 = c.uv - a.uv;

            const float det = d1.x * d2.y - d2.x * d1.y;
            if (std::abs(det) < 1e-8f) continue;   // degenerate UVs, leave the default

            const glm::vec3 tangent = (e1 * d2.y - e2 * d1.y) / det;
            a.tangent += tangent;
            b.tangent += tangent;
            c.tangent += tangent;
        }

        for (auto& v : vertices)
        {
            if (glm::length(v.tangent) > 1e-6f) v.tangent = glm::normalize(v.tangent);
            else v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }
}

std::shared_ptr<Mesh3D> ImportedPrimitive::upload() const
{
    if (vertices.empty() || indices.empty()) return nullptr;

    auto mesh = std::make_shared<Mesh3D>();
    mesh->setName(name);
    if (!mesh->create(vertices, indices)) return nullptr;
    return mesh;
}

bool GltfImporter::isSupported(const std::string& path)
{
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".gltf" || ext == ".glb";
}

std::vector<ImportedPrimitive> GltfImporter::load(const std::string& path)
{
    std::vector<ImportedPrimitive> result;

    if (!std::filesystem::exists(path))
    {
        std::cerr << "[GLTF] file not found: " << path << std::endl;
        return result;
    }

    cgltf_options options{};
    cgltf_data* data = nullptr;

    if (cgltf_parse_file(&options, path.c_str(), &data) != cgltf_result_success)
    {
        std::cerr << "[GLTF] could not parse " << path << std::endl;
        return result;
    }

    // Buffers may be external files or base64 URIs; this resolves both. .glb has them
    // embedded and this is a no-op.
    if (cgltf_load_buffers(&options, data, path.c_str()) != cgltf_result_success)
    {
        std::cerr << "[GLTF] could not load buffers for " << path << std::endl;
        cgltf_free(data);
        return result;
    }

    if (cgltf_validate(data) != cgltf_result_success)
    {
        std::cerr << "[GLTF] " << path << " failed validation." << std::endl;
        cgltf_free(data);
        return result;
    }

    // Walk nodes rather than meshes, so each primitive carries the world transform of
    // the node instancing it. A mesh referenced by two nodes correctly yields two
    // primitives at two places.
    for (cgltf_size n = 0; n < data->nodes_count; n++)
    {
        const cgltf_node* node = &data->nodes[n];
        if (node->mesh == nullptr) continue;

        const glm::mat4 transform = worldTransform(node);

        for (cgltf_size p = 0; p < node->mesh->primitives_count; p++)
        {
            const cgltf_primitive& primitive = node->mesh->primitives[p];

            if (primitive.type != cgltf_primitive_type_triangles)
            {
                // Points, lines and strips exist in the spec but the renderer draws
                // indexed triangles only; skipping is honest, converting would guess.
                continue;
            }

            const cgltf_accessor* positions = nullptr;
            const cgltf_accessor* normals = nullptr;
            const cgltf_accessor* uvs = nullptr;
            const cgltf_accessor* tangents = nullptr;

            for (cgltf_size a = 0; a < primitive.attributes_count; a++)
            {
                const cgltf_attribute& attribute = primitive.attributes[a];
                switch (attribute.type)
                {
                    case cgltf_attribute_type_position: positions = attribute.data; break;
                    case cgltf_attribute_type_normal:   normals = attribute.data; break;
                    case cgltf_attribute_type_tangent:  tangents = attribute.data; break;
                    case cgltf_attribute_type_texcoord:
                        if (attribute.index == 0) uvs = attribute.data;
                        break;
                    default: break;
                }
            }

            if (positions == nullptr) continue;

            const auto pos = readFloats<3>(positions);
            const auto nrm = readFloats<3>(normals);
            const auto uv = readFloats<2>(uvs);
            const auto tan = readFloats<4>(tangents);
            if (pos.empty()) continue;

            std::vector<MeshVertex> vertices(pos.size());
            for (size_t i = 0; i < pos.size(); i++)
            {
                vertices[i].position = pos[i];
                vertices[i].normal = i < nrm.size() ? nrm[i] : glm::vec3(0.0f, 1.0f, 0.0f);
                vertices[i].uv = i < uv.size() ? uv[i] : glm::vec2(0.0f);
                // glTF tangents are vec4; w is the bitangent sign, which the shader
                // does not use yet.
                vertices[i].tangent = i < tan.size() ? glm::vec3(tan[i]) : glm::vec3(0.0f);
            }

            std::vector<uint32_t> indices;
            if (primitive.indices != nullptr)
            {
                indices.resize(primitive.indices->count);
                for (cgltf_size i = 0; i < primitive.indices->count; i++)
                {
                    indices[i] = static_cast<uint32_t>(
                        cgltf_accessor_read_index(primitive.indices, i));
                }
            }
            else
            {
                // Non-indexed geometry is legal; synthesise a trivial index buffer so
                // the renderer has one path rather than two.
                indices.resize(vertices.size());
                for (size_t i = 0; i < indices.size(); i++) indices[i] = static_cast<uint32_t>(i);
            }

            if (tan.empty()) generateTangents(vertices, indices);

            ImportedPrimitive imported;
            imported.vertices = std::move(vertices);
            imported.indices = std::move(indices);
            imported.name = node->name        ? node->name
                          : node->mesh->name  ? node->mesh->name
                                              : "Mesh";
            imported.transform = transform;

            if (primitive.material != nullptr && primitive.material->has_pbr_metallic_roughness)
            {
                const auto& pbr = primitive.material->pbr_metallic_roughness;
                imported.albedo = {pbr.base_color_factor[0], pbr.base_color_factor[1],
                                   pbr.base_color_factor[2], pbr.base_color_factor[3]};
                imported.metallic = pbr.metallic_factor;
                imported.roughness = pbr.roughness_factor;

                const auto& e = primitive.material->emissive_factor;
                imported.emissive = (e[0] + e[1] + e[2]) / 3.0f;
            }

            result.push_back(std::move(imported));
        }
    }

    cgltf_free(data);

    if (result.empty())
    {
        std::cerr << "[GLTF] " << path << " contained no triangle primitives." << std::endl;
    }
    else
    {
        std::cout << "[GLTF] loaded " << result.size() << " primitive(s) from "
                  << std::filesystem::path(path).filename().string() << std::endl;
    }
    return result;
}
