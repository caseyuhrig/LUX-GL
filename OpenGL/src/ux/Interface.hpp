#pragma once

namespace ux {

    class Interface {
    private:
        int windowWidth;
        int windowHeight;
        float aspectRatio;
    public:
        inline Interface() : windowWidth(0), windowHeight(0), aspectRatio(0.0f) {}

        inline void Draw() const
        {

        }
    };
}