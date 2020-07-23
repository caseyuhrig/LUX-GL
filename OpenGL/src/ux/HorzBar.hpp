#pragma once

#include "Primitive.hpp"


namespace ux {

    class HorzBar : public Primitive {
    public:

        inline HorzBar(const glm::vec2& min, const glm::vec2& max)
            : Primitive(glm::vec3(min, 0.0f), glm::vec3(max, 0.0f)) {}


        inline void CreateGeometry()
        {
            glm::vec4 color = GetColor();

            glm::vec2 texCoord = { 0.0f, 0.0f };
            glm::vec3 normal = { 0.0f, 0.0f, 1.0f };

            glm::vec3 center = { (min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, 0.0f };

            _AddVertex(vertices, center, color, texCoord, normal); // 0
            _AddVertex(vertices, { center.x, max.y, 0.0f }, color, texCoord, normal); // 1
            _AddVertex(vertices, { min.x, max.y, 0.0f }, color, texCoord, normal); // 2
            _AddVertex(vertices, { min.x, min.y, 0.0f }, color, texCoord, normal); // 3
            _AddVertex(vertices, { center.x, min.y, 0.0f }, color, texCoord, normal); // 4

            _AddIndex(indicies, 1, 2, 3);
            _AddIndex(indicies, 3, 4, 1);

            int arc_steps = 8;
            float angle_step = 180.0f / (float)arc_steps;
            float arc_radius = center.y - min.y;
            size_t index = 4;

            float angle_start = -90.0 + angle_step;
            float angle_end = angle_start + 180.0;
            // draw the arc from <center.x, min.y> to <center.x, max.y>
            for (float angle = angle_start;angle < angle_end;angle += angle_step)
            {
                float x = center.x + arc_radius * cos(angle * ux::TO_RAD);
                float y = center.y + arc_radius * sin(angle * ux::TO_RAD);
                _AddVertex(vertices, { x, y, 0.0f }, color, texCoord, normal);
                _AddIndex(indicies, 0, index, index + 1);
                index++;
            }

            _AddIndex(indicies, 0, index, 1);
        }
    };
}