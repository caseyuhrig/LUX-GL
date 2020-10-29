#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

//class VertexBufferLayout;

namespace lux {

    class VertexArray
    {
    private:
        uint32_t m_ID;
    public:
        static const Ref<VertexArray> Create() { return CreateRef<VertexArray>(); }

        VertexArray()
        {
            glGenVertexArrays(1, &m_ID);
            glBindVertexArray(m_ID);
        }

        ~VertexArray()
        {
            glDeleteVertexArrays(1, &m_ID);
        }

        void AddBuffer(const Ref<VertexBuffer>& vb, const Ref<VertexBufferLayout>& layout)
        {
            Bind();
            vb->Bind();
            const auto& elements = layout->GetElements();
            uint32_t offset = 0;
            for (auto i = 0; i < elements.size(); i++)
            {
                const auto& element = elements[i];
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, element.count, element.type, element.normalized, layout->GetStride(), make_void_ptr(offset));
                offset += element.count * VertexBufferElement::getSizeOfType(element.type);
            }
        }

        void Bind() const { glBindVertexArray(m_ID); }
        void Unbind() const { glBindVertexArray(0); }
    };
}