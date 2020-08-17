#pragma once

#include "../Primitive/Mesh.hpp"
#include "glm/glm.hpp"


namespace lux {

    class ScoopedCorner : public Mesh
    {
    private:
        //glm::vec3 color;
    public:
        ScoopedCorner(glm::vec2 xy, float size, glm::vec4 _color) 
            : Mesh(glm::vec3(xy.x,xy.y,0.0f),glm::vec3(xy.x+size,xy.y+size,0.0f)) {
            color = _color;
        }

        // 0--1
        // | /
        // 2/
        void CreateGeometry() override
        {
            glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
            AddScoopedCorner(CornerPosition::BottomRight, min, max, normal, color);
                /*
            // top left on the xy plane
            glm::vec3 v0 = { min.x, max.y, 0.0f };
            glm::vec3 center = { max.x, min.y, 0.0f };
            glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
            // draw an arc from the bottom left to the top right.
            // the angle goes 90 deg starting at 90 degrees.
            float steps = 8.0f;
            float step = 90.0f / steps;
            float start_angle = 90.0f;
            float end_angle = start_angle + 90.0f;
            float width = max.x - min.x;
            float height = max.y - min.y;
            glm::vec3 v1 = {
                center.x + width * std::cosf(start_angle * TO_RAD),
                center.y + height * std::sinf(start_angle * TO_RAD),
                0.0f
            };
            for (float angle = start_angle;angle < end_angle;angle += step)
            {
                glm::vec3 v2 = {
                    center.x + width * std::cosf((angle + step) * TO_RADf),
                    center.y + height * std::sinf((angle + step) * TO_RADf),
                    0.0f
                };
                AddTriangle(v0, v1, v2, normal, color);
                v1 = v2;
            }
            */
        };
    };
}