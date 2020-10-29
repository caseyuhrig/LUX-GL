#pragma once

#include <glad/glad.h>
#include <entt/entt.hpp>

#include "lux/Scene/Components.hpp"


namespace lux {

    class Shadows
    {
    public:
        Shadows(const Ref<Scene>& scene)
            : m_Scene(scene)
        {
            glGenFramebuffers(1, &m_DepthMapFBO);
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_DepthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);
            for (uint32_t i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            m_DepthShader = CreateRef<Shader>("res/shaders/point_shadows_depth.glsl");          
        }

        ~Shadows()
        {
            glDeleteFramebuffers(1, &m_DepthMapFBO);
            glDeleteTextures(1, &m_DepthCubemap);
        }

        void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO); }
        void UnBind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        //void SetUseShadows(bool& useShadows) { m_UseShadows = useShadows; }

        void BindTextureCubemap(const uint32_t slot)
        {
            //glActiveTexture(GL_TEXTURE0 + slot);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);
            glBindTextureUnit(slot, m_DepthCubemap);
        }

        void RenderTextureCubemap(const glm::mat4& model, const glm::vec3& lightPos)
        {
            //const auto& camera = m_Scene->GetCamera();
            // 0. create depth cubemap transformation matrices
            // -----------------------------------------------
            auto& zNear = s_Data.SceneData.zNear;
            auto& zFar = s_Data.SceneData.zFar;
            const auto shadowProj = glm::perspective(glm::radians(90.0f), SHADOW_ASPECT, zNear, zFar);
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            // 1. render scene to depth cubemap
            // --------------------------------
            // TODO Should viewport go below bind?          
            Bind();                        
            //glClear(GL_DEPTH_BUFFER_BIT);
            Renderer::ClearDepthBuffer();
            Renderer::SetViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            m_DepthShader->Bind();
            for (uint32_t i = 0; i < 6; ++i)
                m_DepthShader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            m_DepthShader->SetUniform1f("far_plane", zFar);
            m_DepthShader->SetUniformVec3f("lightPos", lightPos);

            const auto& entityView = m_Scene->GetRegistry().view<Mesh, glm::mat4>();
            for (auto entity : entityView)
            {
                const auto& [mesh, transformation] = entityView.get<Mesh, glm::mat4>(entity);
                m_DepthShader->SetUniformMat4f("model", model * transformation);
                mesh.Draw(m_DepthShader);
            }
            UnBind();
        }
    private:
        const uint32_t SHADOW_WIDTH = 2048;
        const uint32_t SHADOW_HEIGHT = 2048;
        const float SHADOW_ASPECT = static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);
        uint32_t m_DepthCubemap;
        uint32_t m_DepthMapFBO;

        Ref<Shader> m_DepthShader;
        Ref<Scene> m_Scene;

        //bool m_UseShadows = true;
    };
}
