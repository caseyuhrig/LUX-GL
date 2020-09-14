#pragma once

#include <glad/glad.h>


namespace lux {

    class Shadows
    {
    public:
        Shadows(Camera& camera) 
            : m_Camera(camera), m_DepthCubemap(0), m_DepthMapFBO(0)
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

        ~Shadows() = default;

        void Bind(glm::vec3 lightPos)
        {
            // 0. create depth cubemap transformation matrices
            // -----------------------------------------------
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), SHADOW_ASPECT, m_Camera.GetZNear(), m_Camera.GetZFar());
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

            // 1. render scene to depth cubemap
            // --------------------------------
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            m_DepthShader->Bind();
            for (uint32_t i = 0; i < 6; ++i)
                m_DepthShader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            m_DepthShader->SetUniform1f("far_plane", m_Camera.GetZFar());
            m_DepthShader->SetUniformVec3f("lightPos", lightPos);
        }

        inline void UnBind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
        inline uint32_t GetDepthCubemap() const { return m_DepthCubemap; }
        inline Ref<Shader> GetDepthShader() const { return m_DepthShader; }

        void SetModelTransformation(const glm::mat4& transformation) const
        {
            m_DepthShader->SetUniformMat4f("model", transformation);
        }

        void SetUseShadows(bool& useShadows)
        {
            m_UseShadows = useShadows;
        }
    private:
        const uint32_t SHADOW_WIDTH = 2048;
        const uint32_t SHADOW_HEIGHT = 2048;
        const float SHADOW_ASPECT = static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);
        uint32_t m_DepthCubemap;
        uint32_t m_DepthMapFBO;
        Ref<Shader> m_DepthShader;
        bool m_UseShadows = true;
        Camera& m_Camera;
    };
}
