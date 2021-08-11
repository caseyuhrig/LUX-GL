#pragma once

#include <pch.hpp>

#include "lux/Scene/Scene.hpp"
#include "lux/Scene/Components.hpp"
#include "lux/Scene/Model.hpp"


namespace lux {

    static uint32_t EntityIndexCount = 0;

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, const Ref<Scene>& scene)
            : m_EntityHandle(handle), m_Scene(scene) {}
        ~Entity() {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }
        /*
        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
            m_Scene->m_Registry.
        }
        */
        template<typename T>
        void RemoveComponent()
        {
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        glm::mat4& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
        const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }

        operator uint32_t () const { return (uint32_t)m_EntityHandle; }
        operator entt::entity() const { return m_EntityHandle; }
        operator bool() const { return (uint32_t)m_EntityHandle && m_Scene; }

        bool operator==(const Entity& other) const
        {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
        
        static entt::entity& MakeMesh(entt::registry& registry, const Ref<Scene>& scene, const std::string& name, 
            const Ref<Mesh>& mesh, const glm::mat4& transform)
        {
            auto entityHandle = registry.create();
            auto entity = lux::Entity(entityHandle, scene);
            entity.AddComponent<IDComponent>();
            entity.AddComponent<IndexComponent>().Index = lux::Entity::CreateIndex();
            entity.AddComponent<TagComponent>(name);
            entity.AddComponent<MeshComponent>(mesh);
            entity.AddComponent<TransformComponent>(transform);
            return entityHandle;
        }

        static entt::entity& MakeModel(entt::registry& registry, const Ref<Scene>& scene, const std::string& name,
            const Ref<Model>& model, const glm::mat4& transform)
        {
            auto entityHandle = registry.create();
            auto entity = lux::Entity(entityHandle, scene);
            entity.AddComponent<IDComponent>();
            entity.AddComponent<IndexComponent>().Index = lux::Entity::CreateIndex();
            entity.AddComponent<TagComponent>(name);
            entity.AddComponent<ModelComponent>(model);
            entity.AddComponent<TransformComponent>(transform);
            return entityHandle;
        }
        
        /**
         * Helper to setup a default renderable mesh entity. 
         */
        /*
        void MakeMesh(const std::string& name, Ref<Mesh> mesh, glm::mat4 transform)
        {
            AddComponent<lux::TagComponent>(name);
            AddComponent<lux::MeshComponent>();
            AddComponent<lux::IndexComponent>().Index = lux::Entity::CreateIndex();
            AddComponent<lux::MeshComponent>(mesh);
            AddComponent<lux::TransformComponent>(transform);
        }
        */
        static uint32_t CreateIndex()
        {
            return EntityIndexCount++;
        }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Ref<Scene> m_Scene = nullptr;

        friend class Scene;
    };
}