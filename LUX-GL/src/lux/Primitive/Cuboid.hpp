#pragma once

#include "Mesh.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"


namespace lux {

    class Cuboid : public Mesh
    {
    public:
        Cuboid(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transform) 
            : Mesh(min, max, transform) 
        {
            Build();
        }

        Cuboid(const glm::vec3& size) 
            : Mesh(glm::vec3(-size.x/2.0f, -size.y/2.0f, -size.z/2.0f), 
                   glm::vec3(size.x/2.0f, size.y/2.0f, size.z/2.0f), 
                   glm::mat4(1.0f))
        {
            Build();
        }

        void CreateGeometry() override
        {
            float verticesBox[] = {
                min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
                max.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
                max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
                max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
                min.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
                min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,

                min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
                max.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
                max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
                max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
                min.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
                min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,

                min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,
                min.x, max.y, min.z, -1.0f,  0.0f,  0.0f,
                min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
                min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
                min.x, min.y, max.z, -1.0f,  0.0f,  0.0f,
                min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,

                max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,
                max.x, max.y, min.z,  1.0f,  0.0f,  0.0f,
                max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
                max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
                max.x, min.y, max.z,  1.0f,  0.0f,  0.0f,
                max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,

                min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
                max.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
                max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
                max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
                min.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
                min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,

                min.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
                max.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
                max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
                max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
                min.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
                min.x, max.y, min.z,  0.0f,  1.0f,  0.0f
            };

            const size_t vb_size = 6 * 6 * 6;

            for (size_t n = 0;n < vb_size;n += 6) 
            {
                glm::vec3 point = { verticesBox[n], verticesBox[n + 1], verticesBox[n + 2] };
                point = transformation * glm::vec4(point, 1.0);
                glm::vec3 normal = { verticesBox[n + 3], verticesBox[n + 4], verticesBox[n + 5] };
                AddVertex(point, glm::vec4(1.0), glm::vec2(0.0), normal);
            }

            for (size_t n = 0;n < 36;n+=3)
                AddIndex(n, n + 1, n + 2);
        }
    };
}