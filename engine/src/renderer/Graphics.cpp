#include "renderer/Graphics.h"
#include "renderer/Renderer.h"

void ScrapGameEngine::Graphics::drawMesh(Mesh* /*mesh*/, RenderParams params)
{
    // The mesh argument is vestigial: every sprite was a unit quad, and the batcher
    // now generates that geometry itself. Kept so existing call sites still compile;
    // it disappears when SpriteRenderer stops allocating a per-sprite Mesh.
    DrawCommand dc{};
    dc.tint = params.tint;
    dc.translation = params.translation;
    dc.rotationZ = params.rotationZ;
    dc.scale = params.scale;
    dc.texture = params.texture;

    Renderer::submitCommand(dc);
}

void ScrapGameEngine::Graphics::drawQuad(RenderParams params)
{
    DrawCommand dc{};
    dc.tint = params.tint;
    dc.translation = params.translation;
    dc.rotationZ = params.rotationZ;
    dc.scale = params.scale;
    dc.texture = params.texture;

    Renderer::submitCommand(dc);
}
