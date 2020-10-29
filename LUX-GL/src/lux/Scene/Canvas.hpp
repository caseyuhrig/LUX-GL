#pragma once

#include "lux/Renderer/Shader.hpp"
#include "lux/Types.hpp"


namespace lux {

    class Canvas
    {
    public:
        Canvas();
        Canvas(const uint32_t width, const uint32_t height, const uint32_t samples = 8);
        ~Canvas();
        void Resize(const uint32_t& width, const uint32_t& height);
        void Bind() const;
        void Unbind() const;
        void Draw() const;
    private:
        uint32_t RGB_FORMAT = GL_RGBA16F;
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
        uint32_t m_BlurAmount = 10;
        float m_Exposure = 1.0f;

        uint32_t m_BloomFBO;
        uint32_t m_Bloom_ColorAttachment;

        uint32_t m_MSAA_FBO;
        uint32_t m_MSAA_ColorAttachment;
    };
}