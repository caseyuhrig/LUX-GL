#pragma once

#include <pch.hpp>

#include "lux/Primitive/Mesh.hpp"

// golden ratio or 0 or phi
// phi = 0.5 * (1 + sqrt(5))
// phi = (1 + sqrt(5)) / 2
// phi = 2 * cos(PI/5)



// where ϕ = (1 + √5)/2 is the golden ratio(also written τ) ≈ 1.618.The edge length is 2/ϕ = √5 − 1. The circumradius is √3.
// ϕ

// Platonic Solid

// small stellated dodecahedron
// https://commons.wikimedia.org/wiki/File:Small_stellated_dodecahedron.png

// great stellated dodecahedron
// https://en.wikipedia.org/wiki/Great_stellated_dodecahedron
// https://en.wikipedia.org/wiki/Great_stellated_dodecahedron#/media/File:Great_stellated_dodecahedron.png

// excavated dodecahedron
// https://commons.wikimedia.org/wiki/File:Excavated_dodecahedron.png


namespace lux {

    class Dodecahedron : public Mesh, public Creatable<Dodecahedron>
    {
    public:
        enum Type { Regular, Small, Great, Excavated };

        Dodecahedron(float radius) : m_Radius(radius)
        {
            Build();
        }

        void CreateGeometry() override
        {
            // I'm gonna lay everything out very simply, since I'm not a math nerd ;)
            // hance, this needs some updates!
            // https://en.wikipedia.org/wiki/Regular_dodecahedron#Cartesian_coordinates
            // The orange vertices lie at (±1, ±1, ±1) and form a cube (dotted lines).
            auto a = glm::vec3(-1.0f, -1.0f, -1.0f);
            auto b = glm::vec3( 1.0f, -1.0f, -1.0f);
            auto c = glm::vec3( 1.0f, -1.0f,  1.0f);
            auto d = glm::vec3(-1.0f, -1.0f,  1.0f);

            auto e = glm::vec3(-1.0f, 1.0f, -1.0f);
            auto f = glm::vec3( 1.0f, 1.0f, -1.0f);
            auto g = glm::vec3( 1.0f, 1.0f,  1.0f);
            auto h = glm::vec3(-1.0f, 1.0f,  1.0f);

            //The green vertices lie at (0, ±ϕ, ±1/ϕ) and form a rectangle on the yz - plane.
            auto i = glm::vec3(0.0f, -PHIf, -1.0f / PHIf);
            auto j = glm::vec3(0.0f,  PHIf, -1.0f / PHIf);
            auto k = glm::vec3(0.0f,  PHIf,  1.0f / PHIf);
            auto l = glm::vec3(0.0f, -PHIf,  1.0f / PHIf);

            // The blue vertices lie at(±1/ϕ, 0, ±ϕ) and form a rectangle on the xz - plane.
            auto m = glm::vec3(-1.0f / PHIf, 0.0f, -PHIf);
            auto n = glm::vec3( 1.0f / PHIf, 0.0f, -PHIf);
            auto o = glm::vec3( 1.0f / PHIf, 0.0f,  PHIf);
            auto p = glm::vec3(-1.0f / PHIf, 0.0f,  PHIf);

            // The pink vertices lie at(±ϕ, ±1/ϕ, 0) and form a rectangle on the xy - plane.
            auto q = glm::vec3(-PHIf, -1.0f / PHIf, 0.0f);
            auto r = glm::vec3( PHIf, -1.0f / PHIf, 0.0f);
            auto s = glm::vec3( PHIf,  1.0f / PHIf, 0.0f);
            auto t = glm::vec3(-PHIf,  1.0f / PHIf, 0.0f);

            //std::cout << "i " << glm::to_string(i) << std::endl;

            //std::cout << "q " << glm::to_string(q) << std::endl;
            //std::cout << "r " << glm::to_string(r) << std::endl;
            //std::cout << "s " << glm::to_string(s) << std::endl;
            //std::cout << "t " << glm::to_string(t) << std::endl;

            // Create the 12 faces from the 20 verticies.  has 30 edges.
            AddPentagon(i, l, c, r, b, 0.0f); // A
            AddPentagon(l, c, o, p, d, 0.0f); // B
            AddPentagon(a, i, l, d,q, 0.0f); // C
            AddPentagon(r, s, g, o,l, 0.0f); // D
            AddPentagon(a, m, n, b,i, 0.0f); // E
            AddPentagon(n, f, s, r,b, 0.0f); // F

            AddPentagon(e, j, k, h, t, 0.0f); // G
            AddPentagon(q, t, h, p, d, 0.0f); // H
            AddPentagon(k, g, o, p, h, 0.0f); // I
            AddPentagon(m, e, t, q, a, 0.0f); // J
            AddPentagon(f, s, g, k, j, 0.0f); // K
            AddPentagon(n, f, j, e, m, 0.0f); // L
        }
    private:
        float m_Radius;
    };
}