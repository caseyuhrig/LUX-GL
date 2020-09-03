#pragma once

#include "../Renderer/Shader.hpp"
#include "../Types.hpp"


namespace lux {

    class Canvas
    {
    public:
        Canvas();
        ~Canvas();

        void Init(uint32_t width, uint32_t height, uint32_t samples = 8);
        void Bind() const;
        void Unbind() const;
        void Draw() const;
    private:
        bool m_Multisample;
        uint32_t m_Samples;
        uint32_t m_FBO;
        uint32_t m_VAO;
        uint32_t m_VBO;
        uint32_t m_ColorAttachment;
        uint32_t m_BrightnessAttachment;
        uint32_t m_DepthAttachment;
        uint32_t m_Width;
        uint32_t m_Height;
        Ref<Shader> m_Shader;
        Ref<Shader> m_ShaderBlur;
        Ref<Shader> m_ShaderBloomFinal;

        uint32_t pingpongFBO[2];
        uint32_t pingpongColorbuffers[2];
        bool m_Bloom = true;
        float m_Exposure = 1.0f;

        uint32_t m_BloomFBO;
        uint32_t m_Bloom_ColorAttachment;

        uint32_t m_MSAA_FBO;
        uint32_t m_MSAA_ColorAttachment;
    };
}