#pragma once

#include <gl/glew.h>

#include <random>

#include <glm/glm.hpp>

#include "lux/Scene/Camera.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Types.hpp"
#include "lux/Log.hpp"


namespace lux {

    class GBuffer
    {
    public:
        GBuffer(const uint32_t width, const uint32_t height)
            : m_Width(width), m_Height(height)
        {
            shaderGeometryPass = Shader::Create("res/shaders/ssao/ssao_geometry.glsl");
            shaderLightingPass = Shader::Create("res/shaders/ssao/ssao_lighting.glsl");
            shaderSSAO = Shader::Create("res/shaders/ssao/ssao.glsl");
            shaderSSAOBlur = Shader::Create("res/shaders/ssao/ssao_blur.glsl");

            // generate sample kernel
            // ----------------------
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
            std::default_random_engine generator;

            for (unsigned int i = 0; i < 64; ++i)
            {
                auto sample = glm::vec3(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
                sample = glm::normalize(sample);
                sample *= randomFloats(generator);
                float scale = float(i) / 64.0f;

                // scale samples s.t. they're more aligned to center of kernel
                scale = lerp(0.1f, 1.0f, scale * scale);
                sample *= scale;
                ssaoKernel.push_back(sample);
            }

            // generate noise texture
            // ----------------------
            for (unsigned int i = 0; i < 16; i++)
            {
                auto noise = glm::vec3(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
                ssaoNoise.push_back(noise);
            }

            initQuad();
            Resize(width, height);
            initialized = true;
        }

        ~GBuffer()
        {
            glDeleteFramebuffers(1, &gBuffer);
            glDeleteTextures(1, &gPosition);
            glDeleteTextures(1, &gNormal);
            glDeleteTextures(1, &gAlbedo);

            glDeleteFramebuffers(1, &ssaoFBO);
            glDeleteFramebuffers(1, &ssaoBlurFBO);
            glDeleteTextures(1, &ssaoColorBuffer);
            glDeleteTextures(1, &ssaoColorBufferBlur);
            glDeleteTextures(1, &noiseTexture);
        }

        void Resize(const uint32_t width, const uint32_t height)
        {
            if (initialized)
            {
                glDeleteFramebuffers(1, &gBuffer);
                glDeleteTextures(1, &gPosition);
                glDeleteTextures(1, &gNormal);
                glDeleteTextures(1, &gAlbedo);

                glDeleteFramebuffers(1, &ssaoFBO);
                glDeleteFramebuffers(1, &ssaoBlurFBO);
                glDeleteTextures(1, &ssaoColorBuffer);
                glDeleteTextures(1, &ssaoColorBufferBlur);
                glDeleteTextures(1, &noiseTexture);
            }
            m_Width = width;
            m_Height = height;

            glGenFramebuffers(1, &gBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            // position color buffer
            glGenTextures(1, &gPosition);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
            // normal color buffer
            glGenTextures(1, &gNormal);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
            // color + specular color buffer
            glGenTextures(1, &gAlbedo);
            glBindTexture(GL_TEXTURE_2D, gAlbedo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
            // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
            uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
            glDrawBuffers(3, attachments);
            // create and attach depth buffer (renderbuffer)
            uint32_t rboDepth;
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
            // finally check if framebuffer is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                spdlog::error("Framebuffer not complete!");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // also create framebuffer to hold SSAO processing stage 
            // -----------------------------------------------------
            
            glGenFramebuffers(1, &ssaoFBO);
            glGenFramebuffers(1, &ssaoBlurFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            
            // SSAO color buffer
            glGenTextures(1, &ssaoColorBuffer);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                spdlog::error("SSAO Framebuffer not complete!");

            // and blur stage
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glGenTextures(1, &ssaoColorBufferBlur);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                spdlog::error("SSAO Blur Framebuffer not complete!");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
           
            glGenTextures(1, &noiseTexture);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        }
        //void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); }
        void UnBind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        void Before(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
        {
            // 1. geometry pass: render scene's geometry/color data into gbuffer
            // -----------------------------------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //const float ZOOM = 45.0f; camera.Zoom
            //const auto aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
            //glm::mat4 projection = glm::perspective(glm::radians(camera.GetAngle()), aspectRatio, camera.GetZNear(), camera.GetZFar());
            //glm::mat4 view = camera.GetView();
            //glm::mat4 model = glm::mat4(1.0f);
            shaderGeometryPass->Bind();
            shaderGeometryPass->SetUniformMat4f("projection", projection);
            shaderGeometryPass->SetUniformMat4f("view", view);
            // room cube
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
            //model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
            //shaderGeometryPass.setMat4("model", model);
            //shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
            //renderCube();
            //shaderGeometryPass.setInt("invertedNormals", 0);
            // backpack model on the floor
            //model = glm::mat4(1.0f);
            //model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
            //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
            //model = glm::scale(model, glm::vec3(1.0f));
            shaderGeometryPass->SetUniformMat4f("model", model);
            //backpack.Draw(shaderGeometryPass);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void After(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPos, const glm::vec4& lightColor)
        {
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 2. generate SSAO texture
            // ------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAO->Bind();
            // Send kernel + rotation 
            for (unsigned int i = 0; i < 64; ++i)
                shaderSSAO->SetUniformVec3f("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            shaderSSAO->SetUniformMat4f("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // 3. blur SSAO texture to remove noise
            // ------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            shaderSSAOBlur->Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
            // -----------------------------------------------------------------------------------------------------
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaderLightingPass->Bind();
            // send light relevant uniforms
            auto lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0));
            shaderLightingPass->SetUniformVec3f("light.Position", lightPosView);
            shaderLightingPass->SetUniformVec3f("light.Color", lightColor);
            // Update attenuation parameters
            const float linear = 0.09;
            const float quadratic = 0.032;
            shaderLightingPass->SetUniform1f("light.Linear", linear);
            shaderLightingPass->SetUniform1f("light.Quadratic", quadratic);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedo);
            glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            renderQuad();
        }
    private:
        uint32_t m_Width, m_Height;
        uint32_t gBuffer, gPosition, gNormal, gAlbedo;
        uint32_t ssaoFBO, ssaoBlurFBO;
        uint32_t ssaoColorBuffer, ssaoColorBufferBlur;
        uint32_t noiseTexture;

        Ref<Shader> shaderSSAO;
        Ref<Shader> shaderSSAOBlur;
        Ref<Shader> shaderGeometryPass;
        Ref<Shader> shaderLightingPass;

        std::vector<glm::vec3> ssaoKernel;
        std::vector<glm::vec3> ssaoNoise;

        bool initialized = false;

        float lerp(float a, float b, float f)
        {
            return a + f * (b - a);
        }

        uint32_t quadVAO, quadVBO;

        void initQuad()
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }

        void renderQuad() const
        {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }
    };
}