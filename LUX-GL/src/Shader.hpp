#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "glm/glm.hpp"


// https://www.khronos.org/opengl/wiki/GLSL_Object

/* Shader is a program that runs on the GPU.  Compiles on the GPU like a normal program. */
/* Vertex and Fragment shaders, there are more. */
/*
    - pipeline -
    Vertex shader   "called once per vertex" where you want the position to be.
    Fragment shader (or pixel shader) "called once per pixel" that gets filled in and decides the color.
                    Think as if you are coloring and shading in the pixels.  Gets called LOTS!
*/


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
    std::string GeometrySource;
};


class Shader 
{
private:
    std::string m_FilePath;
    unsigned int programID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    
    
    inline ShaderProgramSource _ParseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);

        enum class ShaderType
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
        };

        std::string line;
        std::stringstream ss[3];
        ShaderType type = ShaderType::NONE;

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

    inline unsigned int _CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str(); // same as &source[0]
        // whats an lvalue?
        glShaderSource(id, 1, &src, nullptr); // nullptr for length, assums the lines in the src are null terminated.
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            std::cout << "ERROR IN SHADER MAGIC!" << std::endl;
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            // alloca allocates on the stack in the functions stack frame.  The memory is freed when the function completes!
            // malloc allocates on the heap.
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Failed to compile " <<
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                << " shader!" << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(id);
            return 0;
        }
        return id;
    }

    inline unsigned int _CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
    {
        unsigned int program = glCreateProgram();
        unsigned int vs = _CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = _CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
        unsigned int gs = 0;
        if (geometryShader.length() > 0)
        {
            //std::cout << geometryShader << std::endl;
            //gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
            //glAttachShader(program, gs);
        }
        glAttachShader(program, vs);
        glAttachShader(program, fs);

        glLinkProgram(program);
        glValidateProgram(program);

        // delete now because they are attached, may not want to if you are debugging.  Lots of program don't call, minimal impact.
        glDeleteShader(vs);
        glDeleteShader(fs);
        if (gs)
        {
            //glDeleteShader(gs);
        }
        return program;
    }

    inline int _GetUniformLocation(const std::string& name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(programID, name.c_str());
        if (location == -1)
            std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

        m_UniformLocationCache[name] = location;
        return location;
    }
public:

    inline Shader(const std::string& filepath)
        : m_FilePath(filepath), programID(0)
    {
        ShaderProgramSource source = _ParseShader(filepath);
        //std::cout << "VERTEX" << std::endl;
        //std::cout << source.VertexSource << std::endl;
        //std::cout << "FRAGMENT" << std::endl;
        //std::cout << source.FragmentSource << std::endl;
        programID = _CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
    }

    inline ~Shader()
    {
        glDeleteProgram(programID);
    }

    inline void Bind() const
    {
        glUseProgram(programID);
    }


    inline void Unbind() const
    {
        glUseProgram(0);
    }

    inline unsigned int GetProgramID()
    {
        return programID;
    }

    inline void SetUniform1i(const std::string& name, int value)
    {
        //glUniform1i(_GetUniformLocation(name), value);
        glProgramUniform1i(programID, _GetUniformLocation(name), value);
    }


    inline void SetUniform1f(const std::string& name, float value)
    {
        //glUniform1f(_GetUniformLocation(name), value);
        glProgramUniform1f(programID, _GetUniformLocation(name), value);
    }


    inline void SetUniform3f(const std::string& name, float v0, float v1, float v2)
    {
        //glUniform3f(_GetUniformLocation(name), v0, v1, v2);
        glProgramUniform3f(programID, _GetUniformLocation(name), v0, v1, v2);
    }


    inline void SetUniformVec3f(const std::string& name, glm::vec3 v)
    {
        //glUniform3f(_GetUniformLocation(name), v[0], v[1], v[2]);
        glProgramUniform3f(programID, _GetUniformLocation(name), v[0], v[1], v[2]);
    }


    inline void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        //glUniform4f(_GetUniformLocation(name), v0, v1, v2, v3);
        glProgramUniform4f(programID, _GetUniformLocation(name), v0, v1, v2, v3);
    }


    inline void SetUniformVec4f(const std::string& name, glm::vec4& v)
    {
        //glUniform4f(_GetUniformLocation(name), v[0], v[1], v[2], v[3]);
        glProgramUniform4f(programID, _GetUniformLocation(name), v[0], v[1], v[2], v[3]);
    }


    inline void SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
    {
        //glUniformMatrix4fv(_GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
        glProgramUniformMatrix4fv(programID, _GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }
};
