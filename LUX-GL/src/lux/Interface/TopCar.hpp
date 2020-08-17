#pragma once

#include "../Primitive/Mesh.hpp"


namespace lux {

    class TopCar : public Mesh
    {
    public:
        TopCar(const glm::vec2& min, const glm::vec2& max, 
               float sideWidth, float horzHeight, float radius, float scoopRadius, const glm::vec4& color)
            : Mesh(glm::vec3(min, 0.0f), glm::vec3(max, 0.0f)), 
            _SideWidth(sideWidth), _HorzHeight(horzHeight), _Radius(radius), _ScoopRadius(scoopRadius), _Color(color) {}

        TopCar(const glm::vec2& min, const glm::vec2& max, const glm::vec4& color) 
            : Mesh(glm::vec3(min, 0.0f), glm::vec3(max, 0.0f)),
            _SideWidth(80.0f), _HorzHeight(20.0f), _Radius(24.0f), _ScoopRadius(16.0f), _Color(color) {}

    void CreateGeometry() override
    {
        glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
        float r1 = _Radius;
        float r2 = _ScoopRadius;
        float w1 = _SideWidth;
        float w2 = 500.0f;
        float w3 = 100.0f;
        float w4 = 400.0f;
        float w5 = 150.0f;
        float h1 = 200.0f;
        float h2 = 100.f;
        float h3 = 20.0f;
        float h4 = 15.0f;

        float h5 = 6.0f; //10.0f;
        float h6 = 10.0f;
        float h7 = h4;
        float w6 = 500.0f;
        float w7 = w5;

        glm::vec3 a = { min.x, min.y, 0.0f };
        glm::vec3 b = { min.x + w4, min.y + h1, 0.0f };
        glm::vec3 c = { min.x, b.y + h2, 0.0f };
        glm::vec3 d = { min.x + w2, max.y, 0.0f };
        glm::vec3 e = { b.x + w5, a.y, 0.0f };
        glm::vec3 f = { max.x, e.y + h4, 0.0f };
        glm::vec3 g = { d.x + w3, d.y - h3, 0.0f };
        glm::vec3 h = { f.x, d.y, 0.0f };
        glm::vec3 i = { a.x, b.y + h5, 0.0f };
        glm::vec3 j = { a.x + w6, c.y - h6, 0.0f };
        glm::vec3 k = { j.x + w7, i.y, 0.0f };
        glm::vec3 l = { f.x, k.y + h7, 0.0f };

        AddTopCar(c, d, r1, r2, _SideWidth, _HorzHeight, normal, _Color);
        AddBottomCar(a, b, r1, r2, _SideWidth, h4, normal, _Color);
        // middle bar
        AddBottomCar(i, j, 0.0f, r2, w1, h7, normal, _Color);
        AddHorzBar(e, f, normal, _Color);
        AddHorzBar(g, h, normal, _Color);
        AddHorzBar(k, l, normal, _Color);
    }
    private:
        float _SideWidth;
        float _HorzHeight;
        //float _BottomHeight;
        float _Radius = 20.0f;
        float _ScoopRadius = 14.0f;
        glm::vec4 _Color;
    };
}