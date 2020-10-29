#pragma once

#include <glad/glad.h>

#include <memory>
#include <iostream>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "lux/Log.hpp"
#include "lux/Types.hpp"



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


namespace lux {

    class UniformBuffer
    {
    public:
        static Ref<UniformBuffer> Create(const std::string& name, uint32_t bindingPoint, uint32_t data_size, const void* data)
        {
            return CreateRef<UniformBuffer>(name, bindingPoint, data_size, data);
        }

        static Scope<UniformBuffer> Scope(const std::string& name, uint32_t binding_point, uint32_t data_size, const void* data)
        {
            return CreateScope<UniformBuffer>(name, binding_point, data_size, data);
        }

        UniformBuffer(const std::string& name, const uint32_t bindingPoint, const uint32_t dataSize, const void* data)
            : m_Name(name), m_BindingPoint(bindingPoint), m_DataSize(dataSize)
        {
            UX_LOG_INFO("[UniformBuffer]----------------------------");
            UX_LOG_INFO("          Init: %s", m_Name);
            UX_LOG_INFO(" Binding Point: %d", m_BindingPoint);
            UX_LOG_INFO("     Data Size: %d", m_DataSize);

            // Block index will bind when you have to manually link the structs in the GlSL program.
            // @ binding = #, in the uniform struct, the *new* way in std140!
            //unsigned int uniformBlockIndex = glGetUniformBlockIndex(programID, _name.c_str());
            //std::cout << "   Block index: " << uniformBlockIndex << std::endl;
            //glUniformBlockBinding(programID, uniformBlockIndex, _binding_point);

            glGenBuffers(1, &m_RendererID);
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferData(GL_UNIFORM_BUFFER, m_DataSize, NULL, GL_STATIC_DRAW); // GL_DYNAMIC_DRAW
            glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            //glCreateBuffers(1, &m_RendererID);
            //glNamedBufferStorage(m_RendererID, m_DataSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
            glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID, 0, m_DataSize);

            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, m_DataSize, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        ~UniformBuffer() { glDeleteBuffers(1, &m_RendererID); }

        void SetUniformMat4(const std::string& name, const glm::mat4& m)
        {
            auto& uniform = m_Uniforms[name];
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, uniform.offset, uniform.size, &m);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void SetUniformVec4(const std::string& name, const glm::vec4& v)
        {
            auto& field = m_Uniforms[name];
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &v);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void SetUniformVec3(const std::string& name, const glm::vec3& v)
        {
            auto& field = m_Uniforms[name];
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &v);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void SetUniform1f(const std::string& name, float f)
        {
            auto& field = m_Uniforms[name];
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &f);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void SetUniform1i(const std::string& name, uint32_t value)
        {
            auto& field = m_Uniforms[name];
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, field.offset, field.size, &value);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void AddUniform(std::string name, unsigned int offset, unsigned int size)
        {
            if (m_Uniforms.count(name) > 0)
                std::cout << "[ERROR] " << name << " not found in Uniform Buffer " << m_Name <<
                ", NOT ADDING UNIFORM TO BUFFER!" << std::endl;
            else
                m_Uniforms.insert(std::make_pair(name, byte_offset{ offset, size }));
        }

        // Adds the uniform and sets its starting value.
        void AddUniform(std::string name, unsigned int offset, unsigned int size, void* data)
        {
            if (m_Uniforms.count(name) > 0) {
                std::cout << "[ERROR] " << name << " not found in Uniform Buffer " << m_Name <<
                    ", NOT ADDING UNIFORM TO BUFFER!" << std::endl;
            }
            else {
                m_Uniforms.insert(std::make_pair(name, byte_offset{ offset, size }));
                glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
                glBufferSubData(GL_UNIFORM_BUFFER, offset, size, &data);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        }

        void SetData(const void* data)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, m_DataSize, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void Bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID); }
        void Unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
    private:
        uint32_t  m_RendererID;
        uint32_t  m_BindingPoint;
        std::string  m_Name;
        uint32_t  m_DataSize;
        std::unordered_map <std::string, byte_offset> m_Uniforms;
    };
}