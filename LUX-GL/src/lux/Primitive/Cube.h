#pragma once

#include "../Types.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Renderer/Shader.hpp"
#include "../Renderer/VertexBufferLayout.hpp"
#include "../Renderer/VertexBuffer.hpp"
#include "../Renderer/VertexArray.hpp"


namespace lux {

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