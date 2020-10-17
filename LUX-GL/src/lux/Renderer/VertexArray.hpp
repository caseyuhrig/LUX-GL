#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

//class VertexBufferLayout;

namespace lux {

    class VertexArray
    {
    private:
        unsigned int vao_ID;
    public:
        VertexArray()
        {
            glGenVertexArrays(1, &vao_ID);
            glBindVertexArray(vao_ID);
        }

        ~VertexArray()
        {
            glDeleteVertexArrays(1, &vao_ID);
        }

        void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
        {
            Bind();
            vb.Bind();
            const auto& elements = layout.GetElements();
            uint32_t offset = 0;
            for (unsigned int i = 0; i < elements.size(); i++)
            {
                const auto& element = elements[i];
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), make_void_ptr(offset));
                offset += element.count * VertexBufferElement::getSizeOfType(element.type);
            }

        }

        void Bind() const
        {
            glBindVertexArray(vao_ID);
        }

        void Unbind() const
        {
            glBindVertexArray(0);
        }
    };
}