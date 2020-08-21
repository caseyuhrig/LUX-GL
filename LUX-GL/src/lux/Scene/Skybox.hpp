#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../Types.hpp"
#include "../Renderer/Shader.hpp"
#include "Camera.hpp"


namespace lux {

    class Skybox
    {
    public:
        Skybox();
        ~Skybox();
        void Init();
        void SetCamera(const Camera& camera);
        void Draw() const;
        unsigned int LoadTexture(const std::string& path);
        unsigned int LoadCubemap(std::vector<std::string> faces);
    private:
        Ref<Shader> shader;
        Camera m_Camera;
        glm::mat4 m_View;
        unsigned int skyboxVAO, skyboxVBO;
        unsigned int cubemapTexture;
    };
}