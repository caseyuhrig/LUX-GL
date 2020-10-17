#pragma once

#include "../Primitive/Mesh.hpp"
#include "glm/glm.hpp"


namespace lux {

    
    class RoundedCorner : public Mesh
    {
    public:
        RoundedCorner(const CornerPosition& cornerPosition, const glm::vec2& xy, const float size, const glm::vec4& _color)
            : Mesh(glm::vec3(xy.x, xy.y, 0.0f), glm::vec3(xy.x + size, xy.y + size, 0.0f)), m_CornerPosition(cornerPosition) {
            color = _color;
        }

        void CreateGeometry() override
        {
            // top left on the xy plane
            //glm::vec3 v0 = { min.x, min.y + max.y, 0.0f };
            glm::vec3 center = { 0.0f, 0.0f, 0.0f }; // = { min.x + max.x, min.y, 0.0f };
            glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
            float start_angle = 0.0f;
            float end_angle = 0.0f;
            switch (m_CornerPosition)
            {
            case CornerPosition::TopRight:
                // bottom left is the center point
                center = { min.x, min.y, 0.0f };
                start_angle = 0.0f;
                end_angle = 90.0f;
                break;
            case CornerPosition::TopLeft:
                // bottom left is the center point
                center = { max.x, min.y, 0.0f };
                start_angle = 90.0f;
                end_angle = 180.0f;
                break;
            default:
                UX_LOG_ERROR("Corner Position %d Invalid.", m_CornerPosition);
                break;
            }
            // draw an arc from the bottom left to the top right.
            // the angle goes 90 deg starting at 90 degrees.
            float steps = 8.0f;
            float step = 90.0f / steps;
            float width = max.x - min.x;
            float height = max.y - min.y;
            glm::vec3 v1 = {
                center.x + width * std::cosf(start_angle * TO_RADf),
                center.y + height * std::sinf(start_angle * TO_RADf),
                0.0f
            };
            for (float angle = start_angle;angle < end_angle;angle += step)
            {
                glm::vec3 v2 = {
                    center.x + width * std::cosf((angle + step) * TO_RADf),
                    center.y + height * std::sinf((angle + step) * TO_RADf),
                    0.0f
                };
                AddTriangle(center, v1, v2, normal, color);
                v1 = v2;
            }
        };
    private:
        CornerPosition m_CornerPosition;
    };
}