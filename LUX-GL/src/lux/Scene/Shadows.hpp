#pragma once

#include <glad/glad.h>


namespace lux {

    class Shadows
    {
    public:
        Shadows(Camera& camera) 
            : m_Camera(camera), m_DepthCubemap(0), m_DepthMapFBO(0) {}
        ~Shadows() = default;

        void Init()
        {
            // configure depth map FBO
  // -----------------------
            
            glGenFramebuffers(1, &m_DepthMapFBO);
            // create depth cubemap texture
            
            //glGenTextures(1, &m_DepthCubemap);
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_DepthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubemap);
            for (unsigned int i = 0; i < 6; ++i)
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

            m_Shader = CreateRef<Shader>("res/shaders/point_shadows.glsl");

            
            //shader.Bind();
            m_Shader->SetUniform1i("shadows", m_UseShadows);
        }

        void Before(glm::vec3 lightPos)
        {
            // 0. create depth cubemap transformation matrices
            // -----------------------------------------------
            //float near_plane = 1.0f;
            //float far_plane = camera.GetZFar(); //25.0f;
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, m_Camera.GetZNear(), m_Camera.GetZFar());
            //glm::mat4 shadowProj = glm::infinitePerspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, m_Camera.GetZNear());
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
            for (unsigned int i = 0; i < 6; ++i)
                m_DepthShader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            m_DepthShader->SetUniform1f("far_plane", m_Camera.GetZFar());
            m_DepthShader->SetUniformVec3f("lightPos", lightPos);
        }

        void After() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glViewport(0, 0, m_Camera.GetViewportWidth(), m_Camera.GetViewportHeight());
        }

        uint32_t GetDepthCubemap() { return m_DepthCubemap; }
        Ref<Shader> GetShader() { return m_Shader; }
        Ref<Shader> GetDepthShader() { return m_DepthShader; }
    private:
        uint32_t m_DepthCubemap;
        const uint32_t SHADOW_WIDTH = 2048; //1024;
        const uint32_t SHADOW_HEIGHT = 2048; //1024;
        uint32_t m_DepthMapFBO;
        Ref<Shader> m_Shader;
        Ref<Shader> m_DepthShader;
        bool m_UseShadows = true;
        Camera& m_Camera;
    };
}
