#pragma once

#include <glad/glad.h>


namespace lux {

    class IndexBuffer
    {
    private:
        unsigned int ibo_ID;
        unsigned int m_Count;
    public:
        inline IndexBuffer() : ibo_ID(0), m_Count(0) {}

        inline IndexBuffer(const unsigned int* data, unsigned int count)
            : m_Count(count)
        {
            glGenBuffers(1, &ibo_ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
        }

        ~IndexBuffer()
        {
            glDeleteBuffers(1, &ibo_ID);
        }

        // FIX ME SetData and handle STATIC/DYNAMIC.
        void Init(const unsigned int* data, unsigned int count)
        {
            m_Count = count;
            glGenBuffers(1, &ibo_ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
        }

        inline void Bind() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_ID);
        }

        inline void Unbind() const
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        inline unsigned int GetCount() const { return m_Count; }
    };

}