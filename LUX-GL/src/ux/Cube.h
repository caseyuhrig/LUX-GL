#pragma once

#include "Core.hpp"
#include "../Renderer.hpp"
#include "../Shader.hpp"
#include "../VertexBufferLayout.hpp"

namespace ux {


    class Cube
    {
    private:
        //size_t verticesCount = 6 * 6 * 6;
        //float verticesBox[6 * 6 * 6];
        //static const size_t vertexCount = 36;      
        //unsigned int indiciesBox[36];    
        // cube lines
        //float positionsCubeLines[24];
        //unsigned int indiciesCubeLines[24];
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        // LINES
        Ref<VertexArray> vaLines;
        Ref<VertexBuffer> vbLines;
        Ref<VertexBufferLayout> layoutLines;    
        Ref<IndexBuffer> ibLines;
        glm::mat4 transformation;
        bool has_commited = false;
    public:
        Cube(const glm::vec3& size);
        Cube(const glm::vec3& min, const glm::vec3& max);
        Cube(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transform);
        void commit() const;
        //Cube(float width, float height, float depth);
        inline glm::mat4& Transformation() {
            return transformation;
        }
        void transform(glm::mat4& transformation);
        void Draw(const Renderer& renderer, const Shader& shader) const;
        void DrawOutline(const Renderer& renderer, const Shader& shader) const;
    };

}