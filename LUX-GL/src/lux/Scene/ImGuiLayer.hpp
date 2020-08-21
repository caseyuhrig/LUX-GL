#pragma once

#include "Layer.hpp"
#include "../Scene/Material.hpp"
#include "../Window.hpp"
#include "../Scene/Camera.hpp"
#include "../Scene/Light.hpp"
#include "../Renderer/Shader.hpp"


namespace lux {

    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(Window* window);
        
        void Draw() const;
        void Begin();
        void End();

        void Set1(float* scale, glm::vec3* translate, glm::vec3* rotate) {
            _scale1 = scale;
            _translate1 = translate;
            _rotate1 = rotate;
        }
        void Set2(float* scale, glm::vec3* translate, glm::vec3* rotate) {
            _scale2 = scale;
            _translate2 = translate;
            _rotate2 = rotate;
        }
        void SetCamera(Camera* camera) { _camera = camera; }
        void SetMaterial(Material* material) { 
            _material = material;
        }
        void SetLights(Lights* lights) {
            _lights = lights;
          
        }
        void SetShaderBase(Shader* shader) { _shader_base = shader; }

        void SetCubemapSwapFunction(void (*callback_function)(void));

        bool useSkybox = false;
        bool useCubemap = false;
        bool animate = true;
        bool showText = true;
    private:
        Window* _window;
        Camera* _camera;
        Lights* _lights;
        Material* _material;
        UniformBuffer* _lights_UBO;
        Shader* _shader_base;

        float* _scale1;
        glm::vec3* _translate1;
        glm::vec3* _rotate1;

        float* _scale2;
        glm::vec3* _translate2;
        glm::vec3* _rotate2;

        
    };
}