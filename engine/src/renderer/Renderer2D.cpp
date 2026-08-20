#include "renderer/Renderer2D.h"
#include "renderer/Texture2D.h"
#include "rhi/Buffer.h"
#include "rhi/Shader.h"
#include "rhi/VertexArray.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace ScrapGameEngine;

namespace
{
    constexpr unsigned int kMaxQuads = 10000;
    constexpr unsigned int kMaxVertices = kMaxQuads * 4;
    constexpr unsigned int kMaxIndices = kMaxQuads * 6;

    // GL 3.3 guarantees at least 16 fragment texture units. The real limit is queried
    // at init and this is the ceiling we index the slot array with.
    constexpr unsigned int kMaxTextureSlots = 16;

    struct QuadVertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoord;
        float texIndex;
        int entityId;   ///< Written to the id attachment for picking.
    };

    struct Renderer2DData
    {
        VertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        Shader shader;

        std::vector<QuadVertex> vertices;
        unsigned int quadCount = 0;

        std::array<unsigned int, kMaxTextureSlots> textureSlots{};
        unsigned int textureSlotCount = 1;   // slot 0 is the white texture
        unsigned int whiteTexture = 0;
        unsigned int usableSlots = kMaxTextureSlots;

        glm::mat4 viewProjection{1.0f};
        bool initialized = false;
        bool sceneActive = false;

        Renderer2DStats stats;
    };

    Renderer2DData s;

    // Unit quad corners, centred on the origin so a transform's translation is its centre.
    constexpr glm::vec4 kQuadPositions[4] = {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        { 0.5f, -0.5f, 0.0f, 1.0f},
        { 0.5f,  0.5f, 0.0f, 1.0f},
        {-0.5f,  0.5f, 0.0f, 1.0f},
    };
    constexpr glm::vec2 kQuadTexCoords[4] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    };

    const char* kVertexSource = R"(#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityId;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_TexIndex;
flat out int v_EntityId;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = int(a_TexIndex);
    v_EntityId = a_EntityId;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
)";

    // The sampler array is indexed through a switch rather than a dynamic subscript:
    // GLSL 330 requires a constant expression for sampler array indexing.
    const char* kFragmentSource = R"(#version 330 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityId;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_TexIndex;
flat in int v_EntityId;

uniform sampler2D u_Textures[16];

void main()
{
    vec4 sampled = vec4(1.0);
    switch (v_TexIndex)
    {
        case  0: sampled = texture(u_Textures[ 0], v_TexCoord); break;
        case  1: sampled = texture(u_Textures[ 1], v_TexCoord); break;
        case  2: sampled = texture(u_Textures[ 2], v_TexCoord); break;
        case  3: sampled = texture(u_Textures[ 3], v_TexCoord); break;
        case  4: sampled = texture(u_Textures[ 4], v_TexCoord); break;
        case  5: sampled = texture(u_Textures[ 5], v_TexCoord); break;
        case  6: sampled = texture(u_Textures[ 6], v_TexCoord); break;
        case  7: sampled = texture(u_Textures[ 7], v_TexCoord); break;
        case  8: sampled = texture(u_Textures[ 8], v_TexCoord); break;
        case  9: sampled = texture(u_Textures[ 9], v_TexCoord); break;
        case 10: sampled = texture(u_Textures[10], v_TexCoord); break;
        case 11: sampled = texture(u_Textures[11], v_TexCoord); break;
        case 12: sampled = texture(u_Textures[12], v_TexCoord); break;
        case 13: sampled = texture(u_Textures[13], v_TexCoord); break;
        case 14: sampled = texture(u_Textures[14], v_TexCoord); break;
        case 15: sampled = texture(u_Textures[15], v_TexCoord); break;
    }

    o_Color = sampled * v_Color;
    // Discard before writing the id, so a transparent pixel is not pickable.
    if (o_Color.a < 0.001) discard;
    o_EntityId = v_EntityId;
}
)";
}

bool Renderer2D::init()
{
    if (s.initialized) return true;

    int maxUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
    s.usableSlots = static_cast<unsigned int>(
        maxUnits > 0 ? (static_cast<unsigned int>(maxUnits) < kMaxTextureSlots
                            ? static_cast<unsigned int>(maxUnits) : kMaxTextureSlots)
                     : 1);

    if (!s.shader.compile(kVertexSource, kFragmentSource))
    {
        std::cerr << "[RENDERER2D] shader failed to build - nothing will draw." << std::endl;
        return false;
    }

    s.vao.create();
    s.vbo.createDynamic(kMaxVertices * sizeof(QuadVertex));
    s.vbo.setLayout({
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_TexCoord"},
        {ShaderDataType::Float,  "a_TexIndex"},
        {ShaderDataType::Int,    "a_EntityId"},
    });
    s.vao.addVertexBuffer(s.vbo);

    // Quad indices never change, so they are uploaded once as a static buffer.
    std::vector<uint32_t> indices(kMaxIndices);
    for (uint32_t q = 0, i = 0; q < kMaxQuads; q++, i += 4)
    {
        const size_t base = static_cast<size_t>(q) * 6;
        indices[base + 0] = i + 0;
        indices[base + 1] = i + 1;
        indices[base + 2] = i + 2;
        indices[base + 3] = i + 2;
        indices[base + 4] = i + 3;
        indices[base + 5] = i + 0;
    }
    s.ibo.create(indices.data(), kMaxIndices);
    s.vao.setIndexBuffer(s.ibo);

    // A 1x1 opaque white texture in slot 0 lets untextured quads share the same
    // shader path as textured ones - no branch, no second pipeline.
    glGenTextures(1, &s.whiteTexture);
    glBindTexture(GL_TEXTURE_2D, s.whiteTexture);
    const uint32_t white = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    s.textureSlots[0] = s.whiteTexture;

    std::array<int, kMaxTextureSlots> samplers{};
    for (unsigned int i = 0; i < kMaxTextureSlots; i++) samplers[i] = static_cast<int>(i);
    s.shader.bind();
    s.shader.setIntArray("u_Textures", samplers.data(), static_cast<int>(kMaxTextureSlots));

    s.vertices.reserve(kMaxVertices);
    s.initialized = true;

    std::cout << "[RENDERER2D] ready - " << s.usableSlots << " texture slots, "
              << kMaxQuads << " quads per batch." << std::endl;
    return true;
}

void Renderer2D::shutdown()
{
    if (!s.initialized) return;
    if (s.whiteTexture != 0) glDeleteTextures(1, &s.whiteTexture);
    s.whiteTexture = 0;
    s.vertices.clear();
    s.vertices.shrink_to_fit();
    s.initialized = false;
}

void Renderer2D::beginScene(const glm::mat4& viewProjection)
{
    if (!s.initialized) return;

    s.viewProjection = viewProjection;
    s.stats = Renderer2DStats{};
    s.sceneActive = true;
    startBatch();
}

void Renderer2D::startBatch()
{
    s.vertices.clear();
    s.quadCount = 0;
    s.textureSlotCount = 1;
}

void Renderer2D::endScene()
{
    if (!s.initialized) return;
    flush();
    s.sceneActive = false;
}

void Renderer2D::flush()
{
    if (s.quadCount == 0) return;

    s.vbo.setData(s.vertices.data(),
                  static_cast<unsigned int>(s.vertices.size() * sizeof(QuadVertex)));

    s.shader.bind();
    s.shader.setMat4("u_ViewProjection", s.viewProjection);

    for (unsigned int i = 0; i < s.textureSlotCount; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, s.textureSlots[i]);
    }

    s.vao.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(s.quadCount * 6), GL_UNSIGNED_INT, nullptr);
    VertexArray::unbind();

    s.stats.drawCalls++;
}

float Renderer2D::resolveTextureSlot(Texture2D* texture)
{
    if (texture == nullptr || texture->getID() == 0) return 0.0f;   // white texture

    const unsigned int glId = texture->getID();
    for (unsigned int i = 1; i < s.textureSlotCount; i++)
    {
        if (s.textureSlots[i] == glId) return static_cast<float>(i);
    }

    if (s.textureSlotCount >= s.usableSlots)
    {
        // Slots exhausted: close this batch out and start a fresh one so the texture
        // gets a slot rather than being silently dropped.
        flush();
        startBatch();
    }

    const unsigned int slot = s.textureSlotCount;
    s.textureSlots[slot] = glId;
    s.textureSlotCount++;
    return static_cast<float>(slot);
}

void Renderer2D::drawQuad(const glm::mat4& transform, Texture2D* texture,
                          const glm::vec4& tint, int entityId)
{
    if (!s.initialized || !s.sceneActive) return;

    if (s.quadCount >= kMaxQuads)
    {
        flush();
        startBatch();
    }

    const float texIndex = resolveTextureSlot(texture);

    for (int i = 0; i < 4; i++)
    {
        QuadVertex v{};
        v.position = glm::vec3(transform * kQuadPositions[i]);
        v.color = tint;
        v.texCoord = kQuadTexCoords[i];
        v.texIndex = texIndex;
        v.entityId = entityId;
        s.vertices.push_back(v);
    }

    s.quadCount++;
    s.stats.quadCount++;
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    drawQuad(position, size, nullptr, color);
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size,
                          Texture2D* texture, const glm::vec4& tint)
{
    const glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), position) *
        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
    drawQuad(transform, texture, tint);
}

void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
                                 float rotationDegrees, Texture2D* texture, const glm::vec4& tint)
{
    const glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), {0.0f, 0.0f, 1.0f}) *
        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
    drawQuad(transform, texture, tint);
}

const Renderer2DStats& Renderer2D::getStats()
{
    return s.stats;
}
