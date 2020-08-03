#pragma once

#include "glm/glm.hpp"


namespace ux {
    
    static const glm::vec4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
    static const glm::vec4 COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const glm::vec4 COLOR_AMBER = { 1.0f, 0.733f, 0.016f, 1.0f };

    inline static const glm::vec4 color(const glm::vec4& color, const float& alpha)
    {
        return glm::vec4(color[0], color[1], color[2], alpha);
    }
}