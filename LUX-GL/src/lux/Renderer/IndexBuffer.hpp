#pragma once

#include <glad/glad.h>


namespace lux {

    class IndexBuffer
    {
    private:
        uint32_t m_ID; // IBO
        uint32_t m_Count;
    public:
        IndexBuffer() : m_ID(0), m_Count(0) {}

        IndexBuffer(const uint32_t* data, const uint32_t count)
            : m_Count(count)
        {
            glGenBuffers(1, &m_ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
        }

        ~IndexBuffer() { glDeleteBuffers(1, &m_ID); }

        // FIX ME SetData and handle STATIC/DYNAMIC.
        void Init(const uint32_t* data, const uint32_t count)
        {
            m_Count = count;
            glGenBuffers(1, &m_ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
        }

        void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
        void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
        const uint32_t GetCount() const { return m_Count; }
    };

}