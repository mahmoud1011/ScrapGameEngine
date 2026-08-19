#include "rhi/Buffer.h"

#include <glad/glad.h>

namespace ScrapGameEngine
{
    unsigned int shaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Int:    return 4;
            case ShaderDataType::Int2:   return 4 * 2;
            case ShaderDataType::Int3:   return 4 * 3;
            case ShaderDataType::Int4:   return 4 * 4;
            case ShaderDataType::Mat3:   return 4 * 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4 * 4;
            case ShaderDataType::Bool:   return 1;
            case ShaderDataType::None:   break;
        }
        return 0;
    }

    unsigned int shaderDataTypeComponentCount(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Int:    return 1;
            case ShaderDataType::Int2:   return 2;
            case ShaderDataType::Int3:   return 3;
            case ShaderDataType::Int4:   return 4;
            case ShaderDataType::Mat3:   return 3;  // 3 consecutive vec3 attributes
            case ShaderDataType::Mat4:   return 4;  // 4 consecutive vec4 attributes
            case ShaderDataType::Bool:   return 1;
            case ShaderDataType::None:   break;
        }
        return 0;
    }

    void BufferLayout::calculateOffsetsAndStride()
    {
        unsigned int offset = 0;
        stride = 0;
        for (auto& element : elements)
        {
            element.offset = offset;
            offset += element.size;
            stride += element.size;
        }
    }

    // ------------------------------------------------------------- VertexBuffer

    VertexBuffer::~VertexBuffer()
    {
        if (id != 0) glDeleteBuffers(1, &id);
    }

    void VertexBuffer::createDynamic(unsigned int sizeBytes)
    {
        if (id == 0) glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeBytes, nullptr, GL_DYNAMIC_DRAW);
    }

    void VertexBuffer::createStatic(const void* data, unsigned int sizeBytes)
    {
        if (id == 0) glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
    }

    void VertexBuffer::setData(const void* data, unsigned int sizeBytes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeBytes, data);
    }

    void VertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void VertexBuffer::unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // -------------------------------------------------------------- IndexBuffer

    IndexBuffer::~IndexBuffer()
    {
        if (id != 0) glDeleteBuffers(1, &id);
    }

    void IndexBuffer::create(const uint32_t* indices, unsigned int indexCount)
    {
        count = indexCount;
        if (id == 0) glGenBuffers(1, &id);
        // Bound as GL_ARRAY_BUFFER on purpose: binding an element buffer while no
        // vertex array is bound would write into whatever VAO happens to be current.
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void IndexBuffer::unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
