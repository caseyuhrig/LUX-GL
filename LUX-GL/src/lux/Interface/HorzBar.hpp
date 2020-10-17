#pragma once

#include "../Primitive/Mesh.hpp"


namespace lux {

    class HorzBar : public Mesh {
    public:
        HorzBar(const glm::vec2& min, const glm::vec2& max)
            : Mesh(glm::vec3(min, 0.0f), glm::vec3(max, 0.0f)) {}

        void CreateGeometry()
        {
            auto texCoord = glm::vec2(0.0f, 0.0f);
            auto normal = glm::vec3(0.0f, 0.0f, -1.0f);
            auto center = glm::vec3((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, 0.0f);

            AddVertex(center, color, texCoord, normal); // 0
            AddVertex({ center.x, max.y, 0.0f }, color, texCoord, normal); // 1
            AddVertex({ min.x, max.y, 0.0f }, color, texCoord, normal); // 2
            AddVertex({ min.x, min.y, 0.0f }, color, texCoord, normal); // 3
            AddVertex({ center.x, min.y, 0.0f }, color, texCoord, normal); // 4

            AddIndex(1, 2, 3);
            AddIndex(3, 4, 1);

            auto arc_steps = 8.0f;
            auto angle_step = 180.0f / arc_steps;
            auto arc_radius = center.y - min.y;
            uint32_t index = 4;

            auto angle_start = -90.0f + angle_step;
            auto angle_end = angle_start + 180.0f;
            // draw the arc/endcap from <center.x, min.y> to <center.x, max.y>
            for (auto angle = angle_start; angle < angle_end; angle += angle_step)
            {
                auto x = center.x + arc_radius * std::cosf(angle * TO_RADf);
                auto y = center.y + arc_radius * std::sinf(angle * TO_RADf);
                auto vertex = glm::vec3(x, y, 0.0f);
                AddVertex(vertex, color, texCoord, normal);
                AddIndex(0, index, index + 1);
                index++;
            }
            AddIndex(0, index, 1);
        }
    };
}