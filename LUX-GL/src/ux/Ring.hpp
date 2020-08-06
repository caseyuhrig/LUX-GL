#pragma once
#ifndef LUX_RING_HPP
#define LUX_RING_HPP

#include "Mesh.hpp"


namespace ux {
    class Ring : public Mesh
    {
    public:
        Ring(double inner_radius, double outer_radius, double thickness, double steps)
            : Mesh({ -outer_radius, -thickness / 2, -outer_radius }, { -outer_radius, -thickness / 2, -outer_radius }),
            inner_radius(inner_radius), outer_radius(outer_radius), thickness(thickness), steps(steps) {}

        virtual void CreateGeometry() override;
    private:
        float inner_radius;
        float outer_radius;
        float thickness;
        float steps;
    };
}


#endif
