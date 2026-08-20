#include "renderer/Renderer3D.h"
#include "renderer/Mesh3D.h"
#include "rhi/Buffer.h"
#include "rhi/Shader.h"
#include "rhi/VertexArray.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>

#include <array>
#include <iostream>
#include <vector>

using namespace ScrapGameEngine;

namespace
{
    constexpr unsigned int kMaxPointLights = 8;

    struct Renderer3DData
    {
        Shader shader;
        Shader gridShader;
        VertexArray gridVao;
        VertexBuffer gridVbo;
        unsigned int gridVertexCount = 0;

        DirectionalLight sun;
        std::array<PointLight, kMaxPointLights> pointLights{};
        unsigned int pointLightCount = 0;

        glm::mat4 viewProjection{1.0f};
        glm::vec3 cameraPosition{0.0f};

        bool initialized = false;
        bool sceneActive = false;
        bool cullingEnabled = true;
        Frustum frustum;
        Renderer3DStats stats;
    };

    Renderer3DData s;

    const char* kVertexSource = R"(#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_UV;

void main()
{
    vec4 world = u_Model * vec4(a_Position, 1.0);
    v_WorldPos = world.xyz;
    // The inverse-transpose, so non-uniform scale does not shear the normal.
    v_Normal = normalize(u_NormalMatrix * a_Normal);
    v_UV = a_UV;
    gl_Position = u_ViewProjection * world;
}
)";

    // Cook-Torrance with GGX distribution, Smith geometry and Schlick Fresnel - the
    // standard metallic-roughness model, so glTF materials transfer without conversion.
    const char* kFragmentSource = R"(#version 330 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityId;

uniform int u_EntityId;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_UV;

const int MAX_POINT_LIGHTS = 8;
const float PI = 3.14159265359;

uniform vec4  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_Emissive;

uniform vec3  u_CameraPos;
uniform vec3  u_SunDirection;
uniform vec3  u_SunColor;
uniform float u_SunIntensity;

uniform int   u_PointLightCount;
uniform vec3  u_PointPos[MAX_POINT_LIGHTS];
uniform vec3  u_PointColor[MAX_POINT_LIGHTS];
uniform float u_PointIntensity[MAX_POINT_LIGHTS];
uniform float u_PointRange[MAX_POINT_LIGHTS];

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / max(PI * denom * denom, 1e-5);
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    return geometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
           geometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 shade(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 1e-4;
    vec3 specular = numerator / denominator;

    // Metals have no diffuse response.
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    return (kD * albedo / PI + specular) * radiance * max(dot(N, L), 0.0);
}

void main()
{
    vec3 albedo = u_Albedo.rgb;
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_CameraPos - v_WorldPos);

    vec3 color = shade(N, V, normalize(-u_SunDirection),
                       u_SunColor * u_SunIntensity, albedo, u_Metallic, u_Roughness);

    for (int i = 0; i < u_PointLightCount && i < MAX_POINT_LIGHTS; i++)
    {
        vec3 toLight = u_PointPos[i] - v_WorldPos;
        float distance = length(toLight);
        if (distance > u_PointRange[i]) continue;

        // Inverse-square, faded to zero at the range so a light does not pop out.
        float attenuation = 1.0 / (1.0 + distance * distance);
        attenuation *= clamp(1.0 - distance / u_PointRange[i], 0.0, 1.0);

        color += shade(N, V, normalize(toLight),
                       u_PointColor[i] * u_PointIntensity[i] * attenuation,
                       albedo, u_Metallic, u_Roughness);
    }

    color += albedo * 0.03;              // ambient stand-in until IBL exists
    color += albedo * u_Emissive;

    color = color / (color + vec3(1.0)); // Reinhard tonemap
    color = pow(color, vec3(1.0 / 2.2)); // to sRGB

    o_Color = vec4(color, u_Albedo.a);
    o_EntityId = u_EntityId;
}
)";

    const char* kGridVertex = R"(#version 330 core
layout(location = 0) in vec3 a_Position;
uniform mat4 u_ViewProjection;
out vec3 v_Position;
void main()
{
    v_Position = a_Position;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
)";

    const char* kGridFragment = R"(#version 330 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityId;
in vec3 v_Position;
uniform vec3 u_CameraPos;
uniform vec4 u_Color;
uniform float u_Extent;
void main()
{
    // Fade with distance so the grid dissolves instead of ending at a hard edge.
    float d = length(v_Position - vec3(u_CameraPos.x, 0.0, u_CameraPos.z));
    float fade = clamp(1.0 - d / u_Extent, 0.0, 1.0);
    o_Color = vec4(u_Color.rgb, u_Color.a * fade * fade);
    o_EntityId = -1;
}
)";
}

bool Renderer3D::init()
{
    if (s.initialized) return true;

    if (!s.shader.compile(kVertexSource, kFragmentSource))
    {
        std::cerr << "[RENDERER3D] PBR shader failed to build." << std::endl;
        return false;
    }
    if (!s.gridShader.compile(kGridVertex, kGridFragment))
    {
        std::cerr << "[RENDERER3D] grid shader failed to build." << std::endl;
        return false;
    }

    // Grid lines, built once on the XZ plane at one-unit spacing.
    constexpr int halfExtent = 20;
    std::vector<glm::vec3> lines;
    lines.reserve((halfExtent * 2 + 1) * 4);
    for (int i = -halfExtent; i <= halfExtent; i++)
    {
        const auto f = static_cast<float>(i);
        const auto e = static_cast<float>(halfExtent);
        lines.push_back({f, 0.0f, -e});
        lines.push_back({f, 0.0f,  e});
        lines.push_back({-e, 0.0f, f});
        lines.push_back({ e, 0.0f, f});
    }

    s.gridVao.create();
    s.gridVbo.createStatic(lines.data(), static_cast<unsigned int>(lines.size() * sizeof(glm::vec3)));
    s.gridVbo.setLayout({{ShaderDataType::Float3, "a_Position"}});
    s.gridVao.addVertexBuffer(s.gridVbo);
    s.gridVertexCount = static_cast<unsigned int>(lines.size());

    s.initialized = true;
    std::cout << "[RENDERER3D] ready - forward PBR, "
              << kMaxPointLights << " point lights." << std::endl;
    return true;
}

void Renderer3D::shutdown()
{
    s.initialized = false;
}

unsigned int Renderer3D::maxPointLights() { return kMaxPointLights; }

Frustum Frustum::fromViewProjection(const glm::mat4& m)
{
    // Gribb-Hartmann: each plane is a sum or difference of two matrix rows.
    Frustum f;
    for (int i = 0; i < 3; i++)
    {
        f.planes[i * 2 + 0] = glm::vec4(m[0][3] + m[0][i], m[1][3] + m[1][i],
                                        m[2][3] + m[2][i], m[3][3] + m[3][i]);
        f.planes[i * 2 + 1] = glm::vec4(m[0][3] - m[0][i], m[1][3] - m[1][i],
                                        m[2][3] - m[2][i], m[3][3] - m[3][i]);
    }
    for (auto& plane : f.planes)
    {
        const float length = glm::length(glm::vec3(plane));
        if (length > 0.0f) plane /= length;
    }
    return f;
}

bool Frustum::intersectsSphere(const glm::vec3& center, float radius) const
{
    for (const auto& plane : planes)
    {
        if (glm::dot(glm::vec3(plane), center) + plane.w < -radius) return false;
    }
    return true;
}

void Renderer3D::beginScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition)
{
    if (!s.initialized) return;
    s.viewProjection = viewProjection;
    s.cameraPosition = cameraPosition;
    s.frustum = Frustum::fromViewProjection(viewProjection);
    s.stats = Renderer3DStats{};
    s.sceneActive = true;

    // Everything that does not vary per mesh is uploaded once here. Previously all of
    // this - including up to 32 light uniforms - was re-sent for every single mesh,
    // which dominated the draw path in any scene with more than a handful of objects.
    s.shader.bind();
    s.shader.setMat4("u_ViewProjection", viewProjection);
    s.shader.setVec3("u_CameraPos", cameraPosition);
    s.shader.setVec3("u_SunDirection", s.sun.direction);
    s.shader.setVec3("u_SunColor", s.sun.color);
    s.shader.setFloat("u_SunIntensity", s.sun.intensity);
    s.shader.setInt("u_PointLightCount", static_cast<int>(s.pointLightCount));

    for (unsigned int i = 0; i < s.pointLightCount; i++)
    {
        const std::string index = "[" + std::to_string(i) + "]";
        s.shader.setVec3("u_PointPos" + index, s.pointLights[i].position);
        s.shader.setVec3("u_PointColor" + index, s.pointLights[i].color);
        s.shader.setFloat("u_PointIntensity" + index, s.pointLights[i].intensity);
        s.shader.setFloat("u_PointRange" + index, s.pointLights[i].range);
    }
}

const Frustum& Renderer3D::getFrustum() { return s.frustum; }
void Renderer3D::setCullingEnabled(bool enabled) { s.cullingEnabled = enabled; }
bool Renderer3D::isCullingEnabled() { return s.cullingEnabled; }

void Renderer3D::endScene()
{
    s.sceneActive = false;
}

void Renderer3D::setDirectionalLight(const DirectionalLight& light) { s.sun = light; }
void Renderer3D::clearPointLights() { s.pointLightCount = 0; }

void Renderer3D::addPointLight(const PointLight& light)
{
    if (s.pointLightCount >= kMaxPointLights) return;   // silently capped, by design
    s.pointLights[s.pointLightCount++] = light;
}

void Renderer3D::drawMesh(const std::shared_ptr<Mesh3D>& mesh, const glm::mat4& transform,
                          const Material& material, int entityId)
{
    if (!s.initialized || !s.sceneActive || !mesh || !mesh->isValid()) return;

    if (s.cullingEnabled)
    {
        // The primitives are all within a unit radius of their origin, so the world
        // bound is that radius scaled by the largest axis. Conservative, and exact
        // enough that a false accept only costs one wasted draw.
        const glm::vec3 center = glm::vec3(transform[3]);
        const float sx = glm::length(glm::vec3(transform[0]));
        const float sy = glm::length(glm::vec3(transform[1]));
        const float sz = glm::length(glm::vec3(transform[2]));
        const float radius = 0.87f * (sx > sy ? (sx > sz ? sx : sz) : (sy > sz ? sy : sz));

        if (!s.frustum.intersectsSphere(center, radius))
        {
            s.stats.culled++;
            return;
        }
    }

    s.shader.bind();
    s.shader.setMat4("u_Model", transform);

    // glm::inverseTranspose of the upper 3x3, so non-uniform scale keeps normals correct.
    const glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(transform));
    glUniformMatrix3fv(glGetUniformLocation(s.shader.getID(), "u_NormalMatrix"),
                       1, GL_FALSE, &normalMatrix[0][0]);

    s.shader.setVec4("u_Albedo", material.albedo);
    s.shader.setFloat("u_Metallic", material.metallic);
    s.shader.setFloat("u_Roughness", material.roughness);
    s.shader.setFloat("u_Emissive", material.emissive);
    s.shader.setInt("u_EntityId", entityId);
    s.stats.uniformUploads += 6;

    mesh->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->getIndexCount()),
                   GL_UNSIGNED_INT, nullptr);
    VertexArray::unbind();

    s.stats.drawCalls++;
    s.stats.meshCount++;
    s.stats.triangleCount += mesh->getIndexCount() / 3;
}

void Renderer3D::drawGrid(const glm::mat4& viewProjection, float extent)
{
    if (!s.initialized || s.gridVertexCount == 0) return;

    // Depth-tested so geometry occludes it, but not depth-written, so it never
    // occludes anything itself.
    glDepthMask(GL_FALSE);
    s.gridShader.bind();
    s.gridShader.setMat4("u_ViewProjection", viewProjection);
    s.gridShader.setVec3("u_CameraPos", s.cameraPosition);
    s.gridShader.setVec4("u_Color", {0.35f, 0.40f, 0.45f, 0.45f});
    s.gridShader.setFloat("u_Extent", extent);

    s.gridVao.bind();
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(s.gridVertexCount));
    VertexArray::unbind();
    glDepthMask(GL_TRUE);

    s.stats.drawCalls++;
}

const Renderer3DStats& Renderer3D::getStats() { return s.stats; }
