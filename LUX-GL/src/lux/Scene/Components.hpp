#pragma once

#include "lux/Scene/Camera.hpp"
#include "lux/Scene/Model.hpp"


namespace lux {

    struct IDComponent
    {
        UUID ID = 0;
    };

    struct IndexComponent
    {
        uint32_t Index = 0;
    };

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent& other) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}

        operator std::string& () { return Tag; }
        operator const std::string& () const { return Tag; }
        //const boolean equals(const std::string& value) const { return value == Tag; }
        const bool operator == (const std::string& value) { return Tag == value; }
    };

    struct TransformComponent
    {
        glm::mat4 Transform;

        TransformComponent() = default;
        TransformComponent(const TransformComponent& other) = default;
        TransformComponent(const glm::mat4& transform)
            : Transform(transform) {}

        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
    };

    struct MeshComponent
    {
        Ref<lux::Mesh> Mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent& other) = default;
        MeshComponent(const Ref<lux::Mesh>& mesh)
            : Mesh(mesh) {}

        operator Ref<lux::Mesh>() { return Mesh; }
        //operator const Ref<lux::Mesh> () const { return Mesh; }
    };

    struct ModelComponent
    {
        Ref<lux::Model> Model;

        ModelComponent() = default;
        ModelComponent(const ModelComponent& other) = default;
        ModelComponent(const Ref<lux::Model>& model)
            : Model(model) {}

        operator Ref<lux::Model>() { return Model; }
        //operator const Ref<lux::Mesh> () const { return Mesh; }
    };

    struct ColorComponent
    {
        glm::vec4 Color;

        ColorComponent() = default;
        ColorComponent(const ColorComponent& other) = default;
        ColorComponent(const glm::vec4 & color)
            : Color(color) {}

        operator glm::vec4& () { return Color; }
        operator const glm::vec4& () const { return Color; }
    };

    /*
    struct CameraComponent
    {
        Ref<Camera> m_Camera;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const Ref<Camera>& camera) : m_Camera(camera) {}
    };
    */
}