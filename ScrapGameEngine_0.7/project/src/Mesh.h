#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <memory>

namespace ScrapGameEngine
{
    /**
     * @struct Vertex
     * @brief Represents a single vertex with position and UV coordinates.
     *
     * This structure is used to store the position (x, y, z) and UV texture coordinates (u, v)
     * of a vertex for 3D rendering.
     */
    struct Vertex
    {
        float x; ///< X-coordinate of the vertex position.
        float y; ///< Y-coordinate of the vertex position.
        float z; ///< Z-coordinate of the vertex position.
        float u; ///< U-coordinate of the UV texture mapping.
        float v; ///< V-coordinate of the UV texture mapping.

        /**
         * @brief Default constructor for the Vertex struct.
         *
         * Initializes the position to (0, 0, 0) and the UV coordinates to (0, 0).
         */
        Vertex() : x(0), y(0), z(0), u(0), v(0) {}

        /**
         * @brief Constructs a Vertex with specified position and UV coordinates.
         * @param pos The 3D position of the vertex as a glm::vec3.
         * @param uv The UV texture coordinates of the vertex as a glm::vec2.
         */
        Vertex(glm::vec3 pos, glm::vec2 uv) : x(pos.x), y(pos.y), z(pos.z), u(uv.x), v(uv.y) {}
    };

    /**
     * @class Mesh
     * @brief Represents a 3D mesh for rendering.
     *
     * The Mesh class encapsulates vertex data and provides functionality to interact with
     * the underlying graphics API.
     */
    class Mesh
    {
    public:
        /**
         * @brief Constructs a Mesh with the given vertex data.
         * @param vInput A vector of Vertex objects to define the mesh geometry.
         */
        Mesh(std::vector<Vertex> vInput);

        /**
         * @brief Destructor for the Mesh class.
         *
         * Cleans up any allocated resources associated with the mesh.
         */
        ~Mesh();

        /**
         * @brief Gets the unique identifier for the mesh.
         * @return The ID of the mesh as an unsigned int.
         */
        unsigned int getID();

        /**
         * @brief Gets the total number of vertices in the mesh.
         * @return The vertex count of the mesh as an integer.
         */
        int getVertexCount();

    private:
        std::unique_ptr<Mesh> meshData; ///< Pointer to mesh data.
        unsigned int id; ///< Unique identifier for the mesh.
        int vertexCount; ///< The number of vertices in the mesh.
    };
}
