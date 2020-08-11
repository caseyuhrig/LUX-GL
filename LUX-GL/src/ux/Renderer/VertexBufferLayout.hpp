#pragma once

#include <GL/GL.h>
#include <vector>

//#define ASSERT(x) if (!(x)) __debugbreak();


struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:          return 4;
        case GL_UNSIGNED_INT:   return 4;
        case GL_UNSIGNED_BYTE:  return 1;
        }       
        //ASSERT(false);
        return 0;
    }
};


class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    inline VertexBufferLayout()
        : m_Stride(0) {}

    //~VertexBufferLayout();


    template<typename T>
    inline void Push(unsigned int count)
    {
        static_assert(false);
    }


    template<>
    inline void Push<float>(unsigned int count)
    {
        m_Elements.push_back({GL_FLOAT, count, GL_FALSE}); // type, count, normalized
        m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
    }


    template<>
    inline void Push<unsigned int>(unsigned int count)
    {
        m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE }); // type, count, normalized
        m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
    }


    template<>
    inline void Push<unsigned char>(unsigned int count)
    {
        m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE }); // type, count, normalized
        m_Stride += count * VertexBufferElement::getSizeOfType(GL_BYTE);
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }

};