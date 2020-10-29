#pragma once

#include <string>
#include <unordered_map>

#include "lux/Types.hpp"


namespace lux {

    class ShaderStorageBuffer
    {
    public:
        ShaderStorageBuffer(const std::string& name, unsigned int bindingPoint, unsigned int size, const void* data)
            : m_Name(name), m_BindingPoint(bindingPoint), m_Size(size)
        {
            glGenBuffers(1, &m_Renderer_ID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Renderer_ID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindingPoint, m_Renderer_ID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        ~ShaderStorageBuffer()
        {
            glDeleteBuffers(1, &m_Renderer_ID);
        }

        void Publish(uint32_t offset, uint32_t size, const void* data)
        {
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
        }


        void Publish(const void* data)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Renderer_ID);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_Size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void Bind() const { glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Renderer_ID); }

        void Unbind() const
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

    private:
        std::string m_Name;
        uint32_t m_Renderer_ID;
        uint32_t m_BindingPoint;
        uint32_t m_Size;
        std::unordered_map <std::string, byte_offset> m_Layout;
    };
}