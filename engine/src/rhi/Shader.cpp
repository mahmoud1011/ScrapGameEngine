#include "rhi/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

using namespace ScrapGameEngine;

namespace
{
    unsigned int compileStage(unsigned int type, const std::string& source)
    {
        unsigned int stage = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(stage, 1, &src, nullptr);
        glCompileShader(stage);

        int ok = 0;
        glGetShaderiv(stage, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            int length = 0;
            glGetShaderiv(stage, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(static_cast<size_t>(length) + 1, '\0');
            glGetShaderInfoLog(stage, length, nullptr, log.data());

            const char* stageName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
            std::cerr << "[SHADER] " << stageName << " compile failed:\n" << log.data() << std::endl;

            glDeleteShader(stage);
            return 0;
        }
        return stage;
    }
}

Shader::~Shader()
{
    if (id != 0) glDeleteProgram(id);
}

Shader::Shader(Shader&& other) noexcept
    : id(other.id), uniformCache(std::move(other.uniformCache))
{
    other.id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (id != 0) glDeleteProgram(id);
        id = other.id;
        uniformCache = std::move(other.uniformCache);
        other.id = 0;
    }
    return *this;
}

bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource)
{
    unsigned int vs = compileStage(GL_VERTEX_SHADER, vertexSource);
    if (vs == 0) return false;

    unsigned int fs = compileStage(GL_FRAGMENT_SHADER, fragmentSource);
    if (fs == 0)
    {
        glDeleteShader(vs);
        return false;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);

    // The stages are linked into the program now, so they can go either way.
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    if (!ok)
    {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length) + 1, '\0');
        glGetProgramInfoLog(program, length, nullptr, log.data());
        std::cerr << "[SHADER] link failed:\n" << log.data() << std::endl;
        glDeleteProgram(program);
        return false;
    }

    if (id != 0) glDeleteProgram(id);
    id = program;
    uniformCache.clear();
    return true;
}

void Shader::bind() const
{
    glUseProgram(id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

int Shader::uniformLocation(const std::string& name)
{
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) return it->second;

    int location = glGetUniformLocation(id, name.c_str());
    if (location == -1)
    {
        // Not fatal - a uniform the compiler optimised away reports -1, and glUniform
        // on -1 is a documented no-op. Reported once because the cache keeps it.
        std::cerr << "[SHADER] uniform '" << name << "' not found or unused." << std::endl;
    }
    uniformCache[name] = location;
    return location;
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(uniformLocation(name), value);
}

void Shader::setIntArray(const std::string& name, const int* values, int count)
{
    glUniform1iv(uniformLocation(name), count, values);
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(uniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value)
{
    glUniform2fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value)
{
    glUniform4fv(uniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
