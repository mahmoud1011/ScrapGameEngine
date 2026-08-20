#include "renderer/Mesh3D.h"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <iostream>

using namespace ScrapGameEngine;

bool Mesh3D::create(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices)
{
    if (vertices.empty() || indices.empty())
    {
        std::cerr << "[MESH] refusing to upload empty geometry." << std::endl;
        return false;
    }

    vao.create();
    vbo.createStatic(vertices.data(),
                     static_cast<unsigned int>(vertices.size() * sizeof(MeshVertex)));
    vbo.setLayout({
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float3, "a_Normal"},
        {ShaderDataType::Float2, "a_UV"},
        {ShaderDataType::Float3, "a_Tangent"},
    });
    vao.addVertexBuffer(vbo);

    ibo.create(indices.data(), static_cast<unsigned int>(indices.size()));
    vao.setIndexBuffer(ibo);

    indexCount = static_cast<unsigned int>(indices.size());
    return true;
}

std::shared_ptr<Mesh3D> Mesh3D::createCube()
{
    // Built face by face rather than as eight shared corners: a cube needs a distinct
    // normal per face, and a shared vertex can only carry one.
    const glm::vec3 faceNormals[6] = {
        { 0,  0,  1}, { 0,  0, -1}, { 1,  0,  0},
        {-1,  0,  0}, { 0,  1,  0}, { 0, -1,  0},
    };
    const glm::vec3 faceTangents[6] = {
        { 1,  0,  0}, {-1,  0,  0}, { 0,  0, -1},
        { 0,  0,  1}, { 1,  0,  0}, { 1,  0,  0},
    };
    const glm::vec3 corners[6][4] = {
        {{-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}},
        {{ 0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}},
        {{ 0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f, 0.5f}},
        {{-0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f,-0.5f}},
        {{-0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f}},
        {{-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f, 0.5f}, {-0.5f,-0.5f, 0.5f}},
    };
    const glm::vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(24);
    indices.reserve(36);

    for (int face = 0; face < 6; face++)
    {
        const auto base = static_cast<uint32_t>(vertices.size());
        for (int i = 0; i < 4; i++)
        {
            vertices.push_back({corners[face][i], faceNormals[face], uvs[i], faceTangents[face]});
        }
        indices.insert(indices.end(),
                       {base, base + 1, base + 2, base + 2, base + 3, base});
    }

    auto mesh = std::make_shared<Mesh3D>();
    mesh->setName("Cube");
    mesh->create(vertices, indices);
    return mesh;
}

std::shared_ptr<Mesh3D> Mesh3D::createSphere(int segments, int rings)
{
    segments = segments < 3 ? 3 : segments;
    rings = rings < 2 ? 2 : rings;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    for (int y = 0; y <= rings; y++)
    {
        const float v = static_cast<float>(y) / static_cast<float>(rings);
        const float phi = v * glm::pi<float>();

        for (int x = 0; x <= segments; x++)
        {
            const float u = static_cast<float>(x) / static_cast<float>(segments);
            const float theta = u * glm::two_pi<float>();

            const glm::vec3 normal{
                std::sin(phi) * std::cos(theta),
                std::cos(phi),
                std::sin(phi) * std::sin(theta),
            };
            // On a unit sphere the position is the normal, and the tangent runs along
            // the direction of increasing theta.
            vertices.push_back({normal * 0.5f, normal, {u, 1.0f - v},
                                {-std::sin(theta), 0.0f, std::cos(theta)}});
        }
    }

    const auto stride = static_cast<uint32_t>(segments + 1);
    for (int y = 0; y < rings; y++)
    {
        for (int x = 0; x < segments; x++)
        {
            const uint32_t a = static_cast<uint32_t>(y) * stride + static_cast<uint32_t>(x);
            const uint32_t b = a + stride;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }

    auto mesh = std::make_shared<Mesh3D>();
    mesh->setName("Sphere");
    mesh->create(vertices, indices);
    return mesh;
}

std::shared_ptr<Mesh3D> Mesh3D::createPlane(float size)
{
    const float h = size * 0.5f;
    const std::vector<MeshVertex> vertices = {
        {{-h, 0.0f, -h}, {0, 1, 0}, {0, 0}, {1, 0, 0}},
        {{ h, 0.0f, -h}, {0, 1, 0}, {1, 0}, {1, 0, 0}},
        {{ h, 0.0f,  h}, {0, 1, 0}, {1, 1}, {1, 0, 0}},
        {{-h, 0.0f,  h}, {0, 1, 0}, {0, 1}, {1, 0, 0}},
    };
    const std::vector<uint32_t> indices = {0, 2, 1, 2, 0, 3};

    auto mesh = std::make_shared<Mesh3D>();
    mesh->setName("Plane");
    mesh->create(vertices, indices);
    return mesh;
}
