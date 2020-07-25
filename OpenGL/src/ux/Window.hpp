#pragma once

namespace ux {

    class Window {
    private:
        int windowWidth;
        int windowHeight;
        int framebufferWidth;
        int framebufferHeight;
        float aspectRatio;
    public:
        inline Window() : windowWidth(0), windowHeight(0), framebufferWidth(0), framebufferHeight(0), aspectRatio(0.0f) {}

        inline void Draw() const
        {

        }
    };
}