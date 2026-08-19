#include "rhi/VertexArray.h"

#include <glad/glad.h>
#include <iostream>

using namespace ScrapGameEngine;

namespace
{
    GLenum toGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:   return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:   return GL_INT;
            case ShaderDataType::Bool:   return GL_BOOL;
            case ShaderDataType::None:   break;
        }
        return 0;
    }
}

VertexArray::~VertexArray()
{
    if (id != 0) glDeleteVertexArrays(1, &id);
}

void VertexArray::create()
{
    if (id == 0) glGenVertexArrays(1, &id);
}

void VertexArray::bind() const
{
    glBindVertexArray(id);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(const VertexBuffer& buffer)
{
    const auto& layout = buffer.getLayout();
    if (layout.getElements().empty())
    {
        std::cerr << "[VAO] vertex buffer has no layout - nothing to wire." << std::endl;
        return;
    }

    glBindVertexArray(id);
    buffer.bind();

    for (const auto& element : layout)
    {
        const GLenum baseType = toGLBaseType(element.type);
        const auto stride = static_cast<GLsizei>(layout.getStride());

        if (baseType == GL_INT || element.type == ShaderDataType::Bool)
        {
            // Integer attributes must use glVertexAttribIPointer; the float variant
            // would silently convert them.
            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribIPointer(
                attributeIndex,
                static_cast<GLint>(shaderDataTypeComponentCount(element.type)),
                baseType, stride,
                reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
            attributeIndex++;
        }
        else if (element.type == ShaderDataType::Mat3 || element.type == ShaderDataType::Mat4)
        {
            // A matrix occupies one attribute slot per column.
            const unsigned int columns = shaderDataTypeComponentCount(element.type);
            for (unsigned int c = 0; c < columns; c++)
            {
                glEnableVertexAttribArray(attributeIndex);
                glVertexAttribPointer(
                    attributeIndex, static_cast<GLint>(columns), baseType,
                    element.normalized ? GL_TRUE : GL_FALSE, stride,
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(
                        element.offset + sizeof(float) * columns * c)));
                glVertexAttribDivisor(attributeIndex, 1);
                attributeIndex++;
            }
        }
        else
        {
            glEnableVertexAttribArray(attributeIndex);
            glVertexAttribPointer(
                attributeIndex,
                static_cast<GLint>(shaderDataTypeComponentCount(element.type)),
                baseType, element.normalized ? GL_TRUE : GL_FALSE, stride,
                reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
            attributeIndex++;
        }
    }

    glBindVertexArray(0);
}

void VertexArray::setIndexBuffer(const IndexBuffer& buffer)
{
    glBindVertexArray(id);
    buffer.bind();
    indexCount = buffer.getCount();
    glBindVertexArray(0);
}
