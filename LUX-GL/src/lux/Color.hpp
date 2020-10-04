#pragma once

#include "glm/glm.hpp"


namespace lux {
    
    static const glm::vec4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
    static const glm::vec4 COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const glm::vec4 COLOR_AMBER = { 1.0f, 0.733f, 0.016f, 1.0f };

    static const glm::vec4 color(const glm::vec4& color, const float& alpha)
    {
        return glm::vec4(color[0], color[1], color[2], alpha);
    }

    class Color
    {
    public:
        static const glm::vec4 RGBA(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
        {
            float r = static_cast<float>(red) / 255.0f;
            float g = static_cast<float>(green) / 255.0f;
            float b = static_cast<float>(blue) / 255.0f;
            float a = static_cast<float>(alpha) / 255.0f;
            return glm::vec4(r, g, b, a);
        }
        
    private:
    };

    namespace Colors {

        static const glm::vec4 AMBER = Color::RGBA(255, 187, 4, 255);

        // https://www.rapidtables.com/web/color/RGB_Color.html
        static const glm::vec4 DEEP_PINK = Color::RGBA(255, 20, 147, 255);
        static const glm::vec4 BISQUE = Color::RGBA(255, 228, 196, 255);
        static const glm::vec4 STEEL_BLUE = Color::RGBA(70, 130, 180, 255);
        static const glm::vec4 ORANGE = Color::RGBA(255, 165, 0, 255);
        static const glm::vec4 MEDIUM_SPRING_GREEN = Color::RGBA(0, 250, 154, 255);
        /*
        static constexpr glm::vec4 PALETTE[] =
        { 
            AMBER,
            DEEP_PINK,
            MEDIUM_SPRING_GREEN,
            STEEL_BLUE,
            ORANGE
        };
        */
    }
}