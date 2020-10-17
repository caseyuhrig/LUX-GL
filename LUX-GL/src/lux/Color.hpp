#pragma once

#include "glm/glm.hpp"


namespace lux {
    
    class Color
    {
    public:
        static constexpr glm::vec4 RGBA(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
        {
            float r = static_cast<float>(red) / 255.0f;
            float g = static_cast<float>(green) / 255.0f;
            float b = static_cast<float>(blue) / 255.0f;
            float a = static_cast<float>(alpha) / 255.0f;
            return glm::vec4(r, g, b, a);
        }
    };

    namespace Colors {

        constexpr auto White = glm::vec4(1.0f);
        constexpr auto BrightWhite = glm::vec4(1.5f);
        constexpr auto Black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        constexpr auto Amber = Color::RGBA(255, 187, 4, 255);

        // https://www.rapidtables.com/web/color/RGB_Color.html
        // https://www.htmlcsscolor.com/

        constexpr auto DeepPink = Color::RGBA(255, 20, 147, 255);
        constexpr auto NeonPink = Color::RGBA(254, 1, 154, 255);
        constexpr auto Bisque = Color::RGBA(255, 228, 196, 255);
        constexpr auto NeonBlue = Color::RGBA(70, 102, 255, 255);
        constexpr auto SteelBlue = Color::RGBA(70, 130, 180, 255);
        constexpr auto Orange = Color::RGBA(255, 165, 0, 255);
        constexpr auto MediumSpringGreen = Color::RGBA(0, 250, 154, 255);
    }
}