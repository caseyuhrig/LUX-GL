#pragma once

#include <glad/glad.h>
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <iostream>
#include <map>


// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL

// skeletal animation
// std140 vs std430
// std430 Note that this layout can only be used with shader storage blocks, not uniform blocks.
//
// Dynamic Swizzling - Method Swizzling is a dynamic feature that can exchange the implementations of two methods in runtime.
//
// Made a note here because the term just sounds neat and couldn't remeber it the other day and it was driving me crazy.
//
/*
glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);

glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_vertex_uniform_blocks);
glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_fragment_uniform_blocks);
glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_geometry_uniform_blocks);
glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);
*/


class UniformBuffer
{
private:

    typedef struct byte_offset
    {
        unsigned int offset;
        unsigned int size;
    };

    unsigned int  _ubo_ID;
    unsigned int  _binding_point;
    std::string   _name;
    unsigned int  _data_size;
    std::map <std::string, byte_offset> _layout;

public:

    inline UniformBuffer(std::string name, unsigned int binding_point, unsigned int data_size, const void* data) 
        : _name(name), _binding_point(binding_point), _data_size(data_size)
    {
        std::cout << "[UniformBuffer]----------------------------" << std::endl;
        std::cout << "          Init: " << name << std::endl;
        std::cout << " Binding Point: " << _binding_point << std::endl;       
        std::cout << "     Data Size: " << _data_size << std::endl;

        // Block index will bind when you have to manually link the structs in the GlSL program.
        // @ binding = #, in the uniform struct, the *new* way in std140!
        //unsigned int uniformBlockIndex = glGetUniformBlockIndex(programID, _name.c_str());
        //std::cout << "   Block index: " << uniformBlockIndex << std::endl;
        //glUniformBlockBinding(programID, uniformBlockIndex, _binding_point);
        
        glGenBuffers(1, &_ubo_ID);
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferData(GL_UNIFORM_BUFFER, _data_size, NULL, GL_STATIC_DRAW); // GL_DYNAMIC_DRAW
        glBindBufferBase(GL_UNIFORM_BUFFER, _binding_point, _ubo_ID);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, _binding_point, _ubo_ID, 0, _data_size);

        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _data_size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline ~UniformBuffer()
    {
        glDeleteBuffers(1, &_ubo_ID);
    }

    inline void SetUniformMat4(const std::string& name, const glm::mat4& m)
    {
        auto field = _layout[name];
        /*
        if (_layout.count(name) == 0)
        {
            std::cout << "[ERROR] " << name << " not found in Uniform Buffer " << _name << std::endl;
        }
        std::cout << "SetUniform(" << _name << "." << name << ", " << field.offset << ", " << field.size << 
            ") = " << glm::to_string(m) << ", size = " << sizeof(m) << std::endl;
            */
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &m);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void SetUniformVec4(const std::string& name, const glm::vec4& v)
    {
        auto field = _layout[name];
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &v);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void SetUniformVec3(const std::string& name, const glm::vec3& v)
    {
        auto field = _layout[name];
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &v);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void SetUniform1f(const std::string& name, float f)
    {
        auto field = _layout[name];
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &f);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void AddUniform(std::string name, unsigned int offset, unsigned int size)
    {
        if (_layout.count(name) > 0)
            std::cout << "[ERROR] " << name << " not found in Uniform Buffer " << _name << 
                ", NOT ADDING UNIFORM TO BUFFER!" << std::endl;
        else
            _layout.insert(std::make_pair(name, byte_offset { offset, size }));
    }

    // Adds the uniform and sets its starting value.
    inline void AddUniform(std::string name, unsigned int offset, unsigned int size, void* data)
    {
        if (_layout.count(name) > 0) {
            std::cout << "[ERROR] " << name << " not found in Uniform Buffer " << _name <<
                ", NOT ADDING UNIFORM TO BUFFER!" << std::endl;
        }
        else {
            _layout.insert(std::make_pair(name, byte_offset{ offset, size }));
            glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }
    
    inline void SetData(const void* data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, _data_size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    inline void Bind() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo_ID);
    }

    inline void Unbind() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};