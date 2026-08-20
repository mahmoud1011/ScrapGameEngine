#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ScrapGameEngine
{
    /**
     * @enum ShaderDataType
     * @brief Element types a vertex attribute can hold.
     */
    enum class ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        Mat3, Mat4,
        Bool
    };

    unsigned int shaderDataTypeSize(ShaderDataType type);
    unsigned int shaderDataTypeComponentCount(ShaderDataType type);

    /**
     * @struct BufferElement
     * @brief One attribute in a vertex layout.
     */
    struct BufferElement
    {
        std::string name;
        ShaderDataType type = ShaderDataType::None;
        unsigned int size = 0;
        unsigned int offset = 0;
        bool normalized = false;

        BufferElement() = default;
        BufferElement(ShaderDataType type, std::string name, bool normalized = false)
            : name(std::move(name)), type(type), size(shaderDataTypeSize(type)),
              offset(0), normalized(normalized) {}
    };

    /**
     * @class BufferLayout
     * @brief An ordered set of vertex attributes, with offsets and stride computed.
     *
     * Describing the layout in one place is what lets VertexArray wire up the attribute
     * pointers generically, instead of every mesh type hand-rolling its own glVertexAttribPointer calls.
     */
    class BufferLayout
    {
    public:
        BufferLayout() = default;
        BufferLayout(std::initializer_list<BufferElement> elements)
            : elements(elements) { calculateOffsetsAndStride(); }

        unsigned int getStride() const { return stride; }
        const std::vector<BufferElement>& getElements() const { return elements; }

        std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

    private:
        void calculateOffsetsAndStride();

        std::vector<BufferElement> elements;
        unsigned int stride = 0;
    };

    /**
     * @class VertexBuffer
     * @brief A GPU vertex buffer, static or dynamically re-uploaded each frame.
     */
    class VertexBuffer
    {
    public:
        VertexBuffer() = default;
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        /** @brief Allocates an empty dynamic buffer of the given byte size. */
        void createDynamic(unsigned int sizeBytes);

        /** @brief Allocates a static buffer and uploads data into it. */
        void createStatic(const void* data, unsigned int sizeBytes);

        /** @brief Re-uploads into a dynamic buffer. Cheaper than reallocating. */
        void setData(const void* data, unsigned int sizeBytes);

        void bind() const;
        static void unbind();

        void setLayout(const BufferLayout& value) { layout = value; }
        const BufferLayout& getLayout() const { return layout; }

        /**
         * @brief Marks this buffer as holding per-instance data.
         *
         * Every attribute from it then advances once per instance rather than once
         * per vertex. Set on the buffer rather than per element, because mixing the
         * two rates within one buffer is not something any sane layout wants.
         */
        void setInstanced(bool value) { instanced = value; }
        bool isInstanced() const { return instanced; }
        unsigned int getID() const { return id; }

    private:
        unsigned int id = 0;
        BufferLayout layout;
        bool instanced = false;
    };

    /**
     * @class IndexBuffer
     * @brief A GPU index buffer of 32-bit indices.
     */
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        void create(const uint32_t* indices, unsigned int count);
        void bind() const;
        static void unbind();

        unsigned int getCount() const { return count; }

    private:
        unsigned int id = 0;
        unsigned int count = 0;
    };
}
