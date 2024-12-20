#include "Graphics.h"
#include "Renderer.h"
#include <iostream>

void ScrapGameEngine::Graphics::drawMesh(Mesh* _mesh, RenderParams params)
{
    DrawCommand dc{};
    dc.meshId = _mesh->getID();
    dc.vertexStride = sizeof(Vertex);
    dc.vertexCount = _mesh->getVertexCount();
    dc.tint = params.tint;
	dc.translation = params.translation;
	dc.rotationZ = params.rotationZ;
	dc.scale = params.scale;

    // Get the texture ID from the RenderParams
    dc.textureID = params.texture ? params.texture->getID() : 0; // Use texture ID or 0 if no texture

    Renderer::submitCommand(dc); // Submit the draw command
}


