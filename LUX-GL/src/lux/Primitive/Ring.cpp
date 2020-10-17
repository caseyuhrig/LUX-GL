#include "Ring.hpp"

#include "glm/gtx/normal.hpp"


namespace lux {  

    void Ring::CreateGeometry()
    {
        auto color = glm::vec4(1.0f);
        auto step = PI2f / m_Steps;
        for (auto ang = 0.0f; ang < PI2f; ang += step)
        {
            glm::vec3 p1 = glm::vec3(inner_radius * std::cosf(ang), -thickness / 2, inner_radius * std::sinf(ang));
            glm::vec3 p2 = glm::vec3(inner_radius * std::cosf(ang + step), -thickness / 2, inner_radius * std::sinf(ang + step));
            glm::vec3 p3 = glm::vec3(inner_radius * std::cosf(ang), thickness / 2, inner_radius * std::sinf(ang));
            glm::vec3 p4 = glm::vec3(inner_radius * std::cosf(ang + step), thickness / 2, inner_radius * std::sinf(ang + step));

            glm::vec3 p5 = glm::vec3(outer_radius * std::cosf(ang), -thickness / 2, outer_radius * std::sinf(ang));
            glm::vec3 p6 = glm::vec3(outer_radius * std::cosf(ang + step), -thickness / 2, outer_radius * std::sinf(ang + step));
            glm::vec3 p7 = glm::vec3(outer_radius * std::cosf(ang), thickness / 2, outer_radius * std::sinf(ang));
            glm::vec3 p8 = glm::vec3(outer_radius * std::cosf(ang + step), thickness / 2, outer_radius * std::sinf(ang + step));
            //glm::normalize(p1);
            // left side
            AddTriangle(p1, p2, p3, glm::triangleNormal(p1, p2, p3), GetColor());
            AddTriangle(p2, p3, p4, glm::triangleNormal(p2, p3, p4), GetColor());
            // right side
            AddTriangle(p5, p6, p7, glm::triangleNormal(p5, p6, p7), GetColor());
            AddTriangle(p6, p7, p8, glm::triangleNormal(p6, p7, p8), GetColor());
            // top
            AddTriangle(p3, p4, p8, glm::triangleNormal(p3, p4, p8), GetColor());
            AddTriangle(p8, p7, p3, glm::triangleNormal(p8, p7, p3), GetColor());
            // bottom
            AddTriangle(p1, p2, p5, glm::triangleNormal(p1, p2, p5), GetColor());
            AddTriangle(p4, p5, p6, glm::triangleNormal(p4, p5, p6), GetColor());
        }
    }
}