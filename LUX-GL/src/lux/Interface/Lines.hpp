#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "lux/Types.hpp"
#include "lux/Renderer/Renderer.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Renderer/VertexBufferLayout.hpp"


namespace lux {

    struct Line {
        glm::vec3 p1;
        glm::vec3 p2;
    };

    class Lines
    {
    private:
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        std::vector<Line> lines;
        std::vector<glm::vec3> points;
        std::vector<uint32_t> indices;
        uint32_t index = 0;
    public:
        Lines();
        void Add(const glm::vec3& p1, const glm::vec3& p2);
        void CreateGrid(float width, float depth, float xSteps, float zSteps, float y);
        void createCircle(float radius, float steps);
        void createRing(float inner_radius, float outer_radius, float thickness, float steps);
        void Draw(const Renderer& renderer, const Shader& shader) const;
    //protected:
        void Build();
        void Dump();
    };

}