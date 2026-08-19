#pragma once

#include "rhi/Buffer.h"

namespace ScrapGameEngine
{
    /**
     * @class VertexArray
     * @brief A vertex array object binding a vertex buffer's layout to attribute slots.
     *
     * Core-profile OpenGL has no default vertex array, so every draw needs one bound.
     * Attribute wiring is driven by the BufferLayout rather than hand-written per mesh.
     */
    class VertexArray
    {
    public:
        VertexArray() = default;
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void create();
        void bind() const;
        static void unbind();

        /**
         * @brief Binds a vertex buffer and wires its layout to attribute slots.
         *
         * The buffer must already have a layout set, otherwise there is nothing to wire.
         */
        void addVertexBuffer(const VertexBuffer& buffer);

        /** @brief Attaches the index buffer this array draws with. */
        void setIndexBuffer(const IndexBuffer& buffer);

        unsigned int getIndexCount() const { return indexCount; }

    private:
        unsigned int id = 0;
        unsigned int attributeIndex = 0;
        unsigned int indexCount = 0;
    };
}
