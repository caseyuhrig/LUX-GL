#pragma once

#include <math.h>
#include "lux/Primitive/Mesh.hpp"


namespace lux {

    class Plane : public Mesh
    {
    public:
        Plane(float radius, float y, float segments) 
            : Mesh(glm::vec3(-radius, y, -radius), glm::vec3(radius, y, radius)), m_Segments(segments) {}

        void CreateGeometry() override
        {
            auto normal = glm::vec3(0, 1, 0);
            float y = min.y;
            float xStep = (fabsf(min.x) + fabsf(min.x)) / m_Segments;
            float zStep = (fabsf(min.z) + fabsf(max.x)) / m_Segments;
            for (float x = min.x; x < max.x; x += xStep)
            {
                for (float z = min.z; z < max.z; z += zStep)
                {
                    auto a = glm::vec3(x, y, z);
                    auto b = glm::vec3(x + xStep, y, z);
                    auto c = glm::vec3(x + xStep, y, z + zStep);
                    auto d = glm::vec3(x, y, z + zStep);
                    AddQuad(a, b, c, d, normal, color);
                }
            }
        }
    private:
        float m_Segments;
    };
}