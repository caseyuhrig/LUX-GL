#pragma once

#include "Core.hpp"
#include "../Renderer.hpp"
#include "../Shader.hpp"
#include "../VertexBufferLayout.hpp"
#include "glm/glm.hpp";
#include <iostream>
#include <vector>


struct XYZ {
    glm::vec3 point;
    glm::vec3 normal;
};

struct RECT {
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 normal;
};

struct Triangle {
    XYZ a;
    XYZ b;
    XYZ c;
};


namespace ux {

    class Triangles
    {
    private:
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        //std::vector<Triangle> triangles;
        std::vector<XYZ> points;
        std::vector<unsigned int> indices;
        unsigned int index = 0;
    public:
        inline Triangles() {}
        void add(XYZ p1, XYZ p2, XYZ p3);
        void add(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
        void commit();
        void createRing(double inner_radius, double outer_radius, double thickness, double steps);
        void Draw(const Renderer& renderer, const Shader& shader) const;
    };

}