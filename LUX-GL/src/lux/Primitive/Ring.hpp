#pragma once
#ifndef LUX_RING_HPP
#define LUX_RING_HPP

#include "Mesh.hpp"


namespace lux {
    class Ring : public Mesh
    {
    public:
        Ring(float inner_radius, float outer_radius, float thickness, float steps)
            : Mesh({ -outer_radius, -thickness / 2, -outer_radius }, { -outer_radius, -thickness / 2, -outer_radius }),
            inner_radius(inner_radius), outer_radius(outer_radius), thickness(thickness), m_Steps(steps) {}

        virtual void CreateGeometry() override;
    private:
        float inner_radius;
        float outer_radius;
        float thickness;
        float m_Steps;
    };
}


#endif
