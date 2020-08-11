#pragma once

#include "Core.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexBufferLayout.hpp"
#include "glm/glm.hpp";
#include <iostream>
#include <vector>


struct Line {
    glm::vec3 p1;
    glm::vec3 p2;
};


namespace lux {

    class Lines
    {
    private:
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        std::vector<Line> lines;
        std::vector<glm::vec3> points;
        std::vector<unsigned int> indices;
        unsigned int index = 0;
    public:
        Lines();
        void add(glm::vec3 p1, glm::vec3 p2);
        void commit();
        void createCircle(double radius, double steps);
        void createRing(double inner_radius, double outer_radius, double thickness, double steps);
        void Draw(const Renderer& renderer, const Shader& shader) const;
    };

}