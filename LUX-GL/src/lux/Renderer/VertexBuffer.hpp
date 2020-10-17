#pragma once

#include <glad/glad.h>


namespace lux {

    class VertexBuffer
    {
    private:
        unsigned int vbo_ID;
    public:
        VertexBuffer() : vbo_ID(0) {}
        VertexBuffer(const size_t size) : VertexBuffer(nullptr, size, GL_DYNAMIC_DRAW) {}
        VertexBuffer(const void* data, const size_t size) : VertexBuffer(data, size, GL_STATIC_DRAW) {}

        VertexBuffer(const void* data, const size_t size, const uint32_t draw_mode)
        {
            glGenBuffers(1, &vbo_ID);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
        }

        void Init(const void* data, const size_t size, const uint32_t draw_mode = GL_STATIC_DRAW)
        {
            glGenBuffers(1, &vbo_ID);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
        }

        void SetData(const void* data)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
        }

        ~VertexBuffer() { glDeleteBuffers(1, &vbo_ID); }
        void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, vbo_ID); }
        void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    };
}