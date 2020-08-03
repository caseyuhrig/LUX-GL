#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

//class VertexBufferLayout;


class VertexArray
{
private:
    unsigned int vao_ID;
public:
    inline VertexArray()
    {
        glGenVertexArrays(1, &vao_ID);
        glBindVertexArray(vao_ID);
    }

    inline ~VertexArray()
    {
        glDeleteVertexArrays(1, &vao_ID);
    }

    inline void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
    {
        Bind();
        vb.Bind();
        const auto& elements = layout.GetElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); i++)
        {
            const auto& element = elements[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
            offset += element.count * VertexBufferElement::getSizeOfType(element.type);
        }

    }

    inline void Bind() const
    {
        glBindVertexArray(vao_ID);
    }

    inline void Unbind() const
    {
        glBindVertexArray(0);
    }
};