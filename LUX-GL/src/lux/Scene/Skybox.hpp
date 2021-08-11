#pragma once

#include <pch.hpp>

#include "lux/Types.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Scene/Camera.hpp"


namespace lux {

    class Skybox
    {
    public:
        Skybox()
        {
            shader = lux::CreateRef<Shader>("res/shaders/skybox-shader.glsl");
            Init();
        }


        ~Skybox()
        {
            glDeleteVertexArrays(1, &skyboxVAO);
            glDeleteBuffers(1, &skyboxVAO);
        }

        void Init()
        {
            float skyboxVertices[] = {
                // positions          
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f, -1.0f,
                 1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f,  1.0f
            };
            glGenVertexArrays(1, &skyboxVAO);
            glGenBuffers(1, &skyboxVBO);
            glBindVertexArray(skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            std::vector<std::string> faces
            {
                "res/textures/skybox/right.jpg",
                "res/textures/skybox/left.jpg",
                "res/textures/skybox/top.jpg",
                "res/textures/skybox/bottom.jpg",
                "res/textures/skybox/front.jpg",
                "res/textures/skybox/back.jpg",
            };
            /*
            std::vector<std::string> faces
            {
                "res/textures/room/right.jpg",
                "res/textures/room/left.jpg",
                "res/textures/room/top.jpg",
                "res/textures/room/bottom.jpg",
                "res/textures/room/front.jpg",
                "res/textures/room/back.jpg",
            };
            */
            // load textures
            // -------------
            cubemapTexture = LoadCubemap(faces);

            shader->Bind();
            shader->SetUniform1i("skybox", 0);
        }

        void SetCamera(const Ref<Camera>& camera)
        {
            m_Camera = camera;
            glm::vec3 front = { 0.0f, 0.0f, -1.0f };
            glm::vec3 up = { 0.0f, 1.0f, 0.0f };
            glm::mat4 cameraViewMatrix = glm::lookAt(m_Camera->GetPosition(), m_Camera->GetPosition() + front, up);
            m_View = glm::mat4(glm::mat3(cameraViewMatrix)); // remove translation from the view matrix
        }

        void Draw() const
        {
            // draw skybox as last
            glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
            shader->Bind();
            shader->SetUniformMat4f("view", m_View);
            shader->SetUniformMat4f("projection", m_Camera->GetProjection());
            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default
        }


        uint32_t LoadTexture(const std::string& path)
        {
            uint32_t textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format = 0;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                if (format == 0)
                    spdlog::error("LoadTexture: Number of Components: {}???", nrComponents);

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                spdlog::error("Texture failed to load at path: {}", path);
                stbi_image_free(data);
            }
            return textureID;
        }

        // loads a cubemap texture from 6 individual texture faces
    // order:
    // +X (right)
    // -X (left)
    // +Y (top)
    // -Y (bottom)
    // +Z (front) 
    // -Z (back)
    // -------------------------------------------------------
        uint32_t LoadCubemap(std::vector<std::string> faces)
        {
            uint32_t textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            int width, height, nrComponents;
            for (uint32_t i = 0; i < faces.size(); i++)
            {
                unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                }
                else
                {
                    spdlog::error("Cubemap texture failed to load at path: {}", faces[i]);
                    stbi_image_free(data);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            return textureID;
        }
    private:
        Ref<Shader> shader;
        Ref<Camera> m_Camera;
        glm::mat4 m_View;
        uint32_t skyboxVAO;
        uint32_t skyboxVBO;
        uint32_t cubemapTexture;
    };
}