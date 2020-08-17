#pragma once

#include "../Primitive/Mesh.hpp"
//#include "glm/glm.hpp"


namespace lux {


    class Quad : public Mesh
    {
    public:
        Quad(const glm::vec3& p, const glm::vec2& size, const glm::vec4& color)
            : Mesh(glm::vec3(p.x, p.y, 0.0f), glm::vec3(p.x + size.x, p.y + size.y, 0.0f)), _Color(color) 
        {
            
        }

        void CreateGeometry() override
        {
            float z = min.z;
            glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
            glm::vec3 p0 = min;
            glm::vec3 p1 = { max.x, min.y, z };
            glm::vec3 p2 = max;
            glm::vec3 p3 = { min.x, max.y, z };
            //UX_LOG_DEBUG("Adding Quad: %f %f %f\n", min.x, min.y, min.z);
            AddQuad(p0, p1, p2, p3, normal, _Color);
        }
    private:
        glm::vec4 _Color;
    };
}