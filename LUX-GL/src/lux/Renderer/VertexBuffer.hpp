#pragma once

#include <glad/glad.h>


namespace lux {

    class VertexBuffer
    {
    private:
        unsigned int vbo_ID;
    public:
        inline VertexBuffer() : vbo_ID(0) {}
        inline VertexBuffer(unsigned int size) : VertexBuffer(nullptr, size, GL_DYNAMIC_DRAW) {}
        inline VertexBuffer(const void* data, unsigned int size) : VertexBuffer(data, size, GL_STATIC_DRAW) {}

        inline VertexBuffer(const void* data, unsigned int size, int draw_mode)
        {
            glGenBuffers(1, &vbo_ID);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
        }

        inline void Init(const void* data, unsigned int size, int draw_mode = GL_STATIC_DRAW)
        {
            glGenBuffers(1, &vbo_ID);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
        }


        inline void SetData(const void* data)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
        }


        inline ~VertexBuffer()
        {
            glDeleteBuffers(1, &vbo_ID);
        }


        inline void Bind() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
        }


        inline void Unbind() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };
}