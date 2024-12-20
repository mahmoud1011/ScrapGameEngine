#include "Mesh.h"
#include <glad/glad.h>

ScrapGameEngine::Mesh::Mesh(std::vector<Vertex> vInput)
    : id(0), vertexCount(0) // Initialize id and vertexCount to 0
{
    // Assign the size of vInput to vertexCount
    vertexCount = static_cast<unsigned int>(vInput.size());

    // Generate VBO, and upload data to the VBO
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &vInput[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ScrapGameEngine::Mesh::~Mesh()
{
    // Delete the VBO on _mesh destruction.
    glDeleteBuffers(1, &id);
}

unsigned int ScrapGameEngine::Mesh::getID()
{
    // Return the _mesh ID
    return id;  // Assuming 'id' is the OpenGL buffer ID
}

int ScrapGameEngine::Mesh::getVertexCount()
{
    // Return the number of vertices in the _mesh
    return vertexCount; // Returning the vertex count
}
