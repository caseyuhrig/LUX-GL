#pragma once

#include "Shader.hpp"
#include "../Types.hpp"


namespace lux {

    class Canvas
    {
    public:
        Canvas();
        ~Canvas();

        void Init(uint32_t width, uint32_t height, uint32_t samples = 8);
        void Begin();
        void End();
        void Render() const;
    private:
        uint32_t m_Samples;
        uint32_t m_FBO;
        uint32_t m_VAO;
        uint32_t m_VBO;
        uint32_t m_ColorAttachment;
        uint32_t m_DepthAttachment;
        uint32_t m_Width;
        uint32_t m_Height;
        Ref<Shader> m_Shader;
    };
}