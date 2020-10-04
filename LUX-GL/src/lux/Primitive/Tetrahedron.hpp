#pragma once

#include <glm/gtx/normal.hpp>

#include "lux/Primitive/Mesh.hpp"


namespace lux {

    class Tetrahedron : public Mesh
    {
    public:
        Tetrahedron()
        {
            Build();
        }

        ~Tetrahedron() = default;

        void CreateGeometry() override
        {
            const auto v1 = glm::vec3(std::sqrtf(8.0f / 9.0f), 0.0f, -1.0f / 3.0f);
            const auto v2 = glm::vec3(-std::sqrtf(2.0f / 9.0f), std::sqrtf(2.0f/3.0f), -1.0f / 3.0f);
            const auto v3 = glm::vec3(-std::sqrtf(2.0f / 9.0f), -std::sqrtf(2.0f / 3.0f), -1.0f / 3.0f);
            const auto v4 = glm::vec3(0.0f, 0.0f, 1.0f);
            
            AddTriangle(v1, v2, v3);
            AddTriangle(v4, v2, v3);
            AddTriangle(v1, v4, v2);
            AddTriangle(v1, v4, v3);
        }
    private:
    };
}
