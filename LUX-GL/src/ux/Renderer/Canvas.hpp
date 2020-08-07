#pragma once

#include "../../Shader.hpp"
#include "../../ux/Core.hpp"


namespace lux {

    class Canvas
    {
    public:
        Canvas();
        ~Canvas();

        void Init(uint32_t width, uint32_t height);
        void Begin();
        void End();
        void Render() const;
    private:
        uint32_t m_Samples = 8;
        uint32_t m_FBO;
        uint32_t m_VAO;
        uint32_t m_VBO;
        uint32_t m_ColorAttachment;
        uint32_t m_DepthAttachment;
        uint32_t m_Width;
        uint32_t m_Height;
        ux::Ref<Shader> m_Shader;
    };
}