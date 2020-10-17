#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "lux/Interface/Lines.hpp"


namespace lux {

    namespace Primitive {

        class BezierCurve : public Lines
        {
        public:
            BezierCurve() //const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3 d, float time)
            {
                std::vector<glm::vec3> CP;
                
                CP.push_back(glm::vec3(500, 500, 0)); // A
                CP.push_back(glm::vec3(400, 300, 0)); // B
                CP.push_back(glm::vec3(600, 400, 0)); // C
                CP.push_back(glm::vec3(600, 500, 0)); // D
                CP.push_back(glm::vec3(600, 600, 0)); // E
                CP.push_back(glm::vec3(700, 600, 0)); // F
                CP.push_back(glm::vec3(700, 500, 0)); // G
                CP.push_back(glm::vec3(700, 400, 0)); // H
                CP.push_back(glm::vec3(800, 400, 0)); // I
                CP.push_back(glm::vec3(800, 500, 0)); // J
                auto step = 1.0f / 50.0f;
                for (auto t = 0.0f;t < 1.0f;t+=step)
                {
                    auto P1 = cubic_bspline_10(CP, t);
                    auto P2 = cubic_bspline_10(CP, t + step);
                    Add(P1, P2);
                }
                Build();
                //UX_LOG_DEBUG("BezierCurve...");
                //Dump();
            }
        private:
            static const glm::vec3 quadratic_bezier(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float t)
            {
                auto D = glm::mix(A, B, t);
                auto E = glm::mix(B, C, t);
                auto P = glm::mix(D, E, t);
                return P;
            }
            
            static const glm::vec3 cubic_bezier(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& D, float t)
            {
                auto E = glm::mix(A, B, t);
                auto F = glm::mix(B, C, t);
                auto G = glm::mix(C, D, t);
                return quadratic_bezier(E, F, G, t);
            }

            static const glm::vec3 cubic_bspline_10(const std::vector<glm::vec3>& CP, float t)
            {
                auto f = t * 3.0f;
                auto i = static_cast<size_t>(glm::floor(f));
                auto s = glm::fract(f);
                if (t <= 0.0f) return CP[0];
                if (t >= 1.0f) return CP[9];
                auto A = CP[i * 3];
                auto B = CP[i * 3 + 1];
                auto C = CP[i * 3 + 2];
                auto D = CP[i * 3 + 3];
                return cubic_bezier(A, B, C, D, s);
            }
        };
    }
}