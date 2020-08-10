#include "ShaderStorageBuffer.hpp"

#include <glad/glad.h>


namespace lux {

    ShaderStorageBuffer::ShaderStorageBuffer(std::string name, unsigned int binding_point, unsigned int size, const void* data)
        : _name(name), _binding_point(binding_point), _size(size)
    {
        glGenBuffers(1, &_ssbo_ID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo_ID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding_point, _ssbo_ID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ShaderStorageBuffer::~ShaderStorageBuffer()
    {
        glDeleteBuffers(1, &_ssbo_ID);
    }

    void ShaderStorageBuffer::Publish(uint32_t offset, uint32_t size, const void* data)
    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }


    void ShaderStorageBuffer::Publish(const void* data)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo_ID);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _size, data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void ShaderStorageBuffer::Bind() const
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo_ID);
    }

    void ShaderStorageBuffer::Unbind() const
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    
}