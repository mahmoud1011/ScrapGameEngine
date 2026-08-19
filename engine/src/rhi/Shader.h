#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace ScrapGameEngine
{
    /**
     * @class Shader
     * @brief A linked GPU program, built from vertex and fragment source.
     *
     * Uniform locations are looked up once and cached, since glGetUniformLocation is a
     * string lookup on the driver side and the renderer sets uniforms every frame.
     */
    class Shader
    {
    public:
        Shader() = default;
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;

        /**
         * @brief Compiles and links a program from source.
         * @return True on success. On failure the compile or link log is reported and
         *         the shader stays unusable rather than silently binding nothing.
         */
        bool compile(const std::string& vertexSource, const std::string& fragmentSource);

        void bind() const;
        static void unbind();

        bool isValid() const { return id != 0; }
        unsigned int getID() const { return id; }

        void setInt(const std::string& name, int value);
        void setIntArray(const std::string& name, const int* values, int count);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat4(const std::string& name, const glm::mat4& value);

    private:
        int uniformLocation(const std::string& name);

        unsigned int id = 0;
        std::unordered_map<std::string, int> uniformCache;
    };
}
