#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../Types.hpp"
#include "../Renderer/Shader.hpp"


namespace lux {

    class Skybox
    {
    public:
        Skybox();
        ~Skybox();
        void Init();
        void Render(const glm::vec3& cameraPosition, const glm::mat4& projection);
        unsigned int LoadTexture(const std::string& path);
        unsigned int LoadCubemap(std::vector<std::string> faces);
    private:
        Ref<Shader> shader;
        unsigned int skyboxVAO, skyboxVBO;
        unsigned int cubemapTexture;
    };
}