#pragma once

#include "lux/Scene/Camera.hpp"


namespace lux {

    struct CameraComponent
    {
        Ref<Camera> m_Camera;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const Ref<Camera>& camera) : m_Camera(camera) {}
    };
}