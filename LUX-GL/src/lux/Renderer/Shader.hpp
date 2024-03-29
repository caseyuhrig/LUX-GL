#pragma once

#include <gl/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "glm/glm.hpp"

#include "lux/Types.hpp"
#include "lux/Log.hpp"


// https://www.khronos.org/opengl/wiki/GLSL_Object

/* Shader is a program that runs on the GPU.  Compiles on the GPU like a normal program. */
/* Vertex and Fragment shaders, there are more. */
/*
    - pipeline -
    Vertex shader   "called once per vertex" where you want the position to be.
    Fragment shader (or pixel shader) "called once per pixel" that gets filled in and decides the color.
                    Think as if you are coloring and shading in the pixels.  Gets called LOTS!
*/

namespace lux {

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
    };

    struct ShaderProgramSource
    {
        std::string VertexSource;
        std::string FragmentSource;
        std::string GeometrySource;
    };


    class Shader : public Creatable<Shader>
    {
    public:
        Shader(const std::string& filepath)
            : m_FilePath(filepath), programID(0)
        {
            const ShaderProgramSource source = ParseShader(filepath);
            programID = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
        }

        ~Shader()
        {
            glDeleteProgram(programID);
        }

        void Bind() const { glUseProgram(programID); }
        void Unbind() const { glUseProgram(0); }
        const uint32_t GetProgramID() const { return programID; }

        auto Duplicate() { return Shader::Create(m_FilePath); }

        /**
        * glProgramUniform* vs. glUniform*
        * the glProgram* version alloes you to set the uniform value without having to
        * bind the program first.
        */

        void SetUniform1i(const std::string& name, int value)
        {
            glProgramUniform1i(programID, GetUniformLocation(name), value);
        }


        void SetUniform1f(const std::string& name, float value)
        {
            glProgramUniform1f(programID, GetUniformLocation(name), value);
        }


        void SetUniform3f(const std::string& name, float v0, float v1, float v2)
        {
            glProgramUniform3f(programID, GetUniformLocation(name), v0, v1, v2);
        }


        void SetUniformVec3f(const std::string& name, glm::vec3 v)
        {
            glProgramUniform3f(programID, GetUniformLocation(name), v[0], v[1], v[2]);
        }


        void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
        {
            glProgramUniform4f(programID, GetUniformLocation(name), v0, v1, v2, v3);
        }


        void SetUniformVec4f(const std::string& name, const glm::vec4& v)
        {
            glProgramUniform4f(programID, GetUniformLocation(name), v[0], v[1], v[2], v[3]);
        }


        void SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
        {
            glProgramUniformMatrix4fv(programID, GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
        }
    private:
        std::string m_FilePath;
        uint32_t programID;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        ShaderProgramSource ParseShader(const std::string& filepath)
        {
            std::ifstream stream(filepath);

            std::string line;
            std::stringstream ss[3];
            auto type = ShaderType::NONE;

            while (getline(stream, line))
            {
                if (line.find("#shader") != std::string::npos)
                {
                    if (line.find("vertex") != std::string::npos)
                        type = ShaderType::VERTEX;
                    else if (line.find("fragment") != std::string::npos)
                        type = ShaderType::FRAGMENT;
                    else if (line.find("geometry") != std::string::npos)
                        type = ShaderType::GEOMETRY;
                }
                else {
                    ss[(int)type] << line << '\n';
                }
            }
            return { ss[0].str(), ss[1].str() , ss[2].str() };
        }

        uint32_t CompileShader(const uint32_t type, const std::string& source)
        {
            const auto id = glCreateShader(type);
            const auto* src = source.c_str(); // same as &source[0]
            // whats an lvalue?
            glShaderSource(id, 1, &src, nullptr); // nullptr for length, assums the lines in the src are null terminated.
            glCompileShader(id);

            int result;
            glGetShaderiv(id, GL_COMPILE_STATUS, &result);
            if (result == GL_FALSE)
            {
                int length;
                glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
                // alloca allocates on the stack in the functions stack frame.  The memory is freed when the function completes!
                // malloc allocates on the heap.
                std::string message;
                glGetShaderInfoLog(id, length, &length, message.data());
                spdlog::error("Failed to compile {} shader!", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
                spdlog::error("{}", message);
                spdlog::error("{}", source);
                glDeleteShader(id);
                return 0;
            }
            return id;
        }

        uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
        {
            auto program = glCreateProgram();
            auto vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
            auto fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
            auto gs = 0;
            if (geometryShader.length() > 0)
            {
                gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
                glAttachShader(program, gs);
            }
            glAttachShader(program, vs);
            glAttachShader(program, fs);

            glLinkProgram(program);
            glValidateProgram(program);

            // Delete now because they are attached, may not want to if you are debugging.
            // Lots of program don't call, minimal impact.
            if (vs) glDeleteShader(vs);
            if (fs) glDeleteShader(fs);
            if (gs) glDeleteShader(gs);
            return program;
        }

        int GetUniformLocation(const std::string& name)
        {
            if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
                return m_UniformLocationCache[name];

            auto location = glGetUniformLocation(programID, name.c_str());
            if (location == -1)
                spdlog::error("Warning: uniform '{}' doesn't exist!", name.c_str());

            m_UniformLocationCache[name] = location;
            return location;
        }
    };
}
