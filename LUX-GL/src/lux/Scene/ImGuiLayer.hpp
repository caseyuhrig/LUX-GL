#pragma once

#include "Layer.hpp"
#include "lux/Scene/Material.hpp"
#include "lux/Window.hpp"
#include "lux/Scene/Camera.hpp"
#include "lux/Scene/Light.hpp"
#include "lux/Renderer/Shader.hpp"


namespace lux {

    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(Window* window);
        
        void Draw() const;
        void Begin();
        void End();

        void Resize(const uint32_t width, const uint32_t height);

        void SetCubeTransforms(glm::vec3* scaleCube, glm::vec3* translateCube, glm::vec3* rotateCube)
        {
            m_ScaleCube = scaleCube;
            m_TranslateCube = translateCube;
            m_RotateCube = rotateCube;
        }

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

        void SetShadowSamples(int* shadowSamples) { m_ShadowSamples = shadowSamples; }
        void SetShadowBias(float* shadowBias) { m_ShadowBias = shadowBias; }

        void SetCamera(Camera* camera) { _camera = camera; }
        void SetMaterial(Material* material) { _material = material; }
        void SetLights(Lights* lights) { _lights = lights; }
        void SetLightPos(glm::vec3* lightPos) { m_LightPos = lightPos; }
        void SetShaderBase(Shader* shader) { _shader_base = shader; }

        void SetCubemapSwapFunction(void (*callback_function)(void));

        bool useShadows = true;
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

        glm::vec3* m_LightPos;

        glm::vec3* m_ScaleCube;
        glm::vec3* m_TranslateCube;
        glm::vec3* m_RotateCube;

        float* _scale1;
        glm::vec3* _translate1;
        glm::vec3* _rotate1;

        float* _scale2;
        glm::vec3* _translate2;
        glm::vec3* _rotate2;

        float* m_ShadowBias;
        int* m_ShadowSamples;
    };
}