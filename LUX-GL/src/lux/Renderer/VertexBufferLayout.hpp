#pragma once

#include <gl/glew.h>
#include <vector>
#include "lux/Types.hpp"


namespace lux {

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
        uint32_t m_Stride;
    public:
        static Ref<VertexBufferLayout> Create() { return CreateRef<VertexBufferLayout>(); }

        VertexBufferLayout()
            : m_Stride(0) {}

        //~VertexBufferLayout();


        template<typename T>
        void Push(unsigned int count)
        {
            static_assert(false);
        }


        template<>
        void Push<float>(unsigned int count)
        {
            m_Elements.push_back({ GL_FLOAT, count, GL_FALSE }); // type, count, normalized
            m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
        }


        template<>
        void Push<unsigned int>(unsigned int count)
        {
            m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE }); // type, count, normalized
            m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
        }


        template<>
        void Push<unsigned char>(unsigned int count)
        {
            m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE }); // type, count, normalized
            m_Stride += count * VertexBufferElement::getSizeOfType(GL_BYTE);
        }

        const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
        uint32_t GetStride() const { return m_Stride; }
    };
}