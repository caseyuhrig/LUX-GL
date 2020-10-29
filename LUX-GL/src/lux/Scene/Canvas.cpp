#include "Canvas.hpp"

#include <glad/glad.h>

#include "lux/Renderer/Renderer.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Log.hpp"


namespace lux {

    Canvas::Canvas(const uint32_t width, const uint32_t height, const uint32_t samples)
        : m_Width(width), m_Height(height), m_Samples(samples), m_Multisample(samples > 1)
    {
        if (m_Multisample)
        {
            m_Shader = Shader::Create("res/shaders/canvas-shader-MSAA.glsl");
            m_Shader->SetUniform1i("u_Samples", m_Samples);
            m_ShaderBloomFinal = Shader::Create("res/shaders/shader-bloom_final-MSAA.glsl");
        }
        else {
            m_Shader = Shader::Create("res/shaders/canvas-shader.glsl");
            m_ShaderBloomFinal = Shader::Create("res/shaders/shader-bloom_final.glsl");
        }

        m_ShaderBlur = Shader::Create("res/shaders/shader-blur.glsl");
        m_ShaderBlur->SetUniform1i("image", 0);
        m_ShaderBloomFinal->Bind();
        m_ShaderBloomFinal->SetUniform1i("scene", 0);
        m_ShaderBloomFinal->SetUniform1i("bloomBlur", 1);

        m_Shader->SetUniform1i("u_ScreenTexture", 0); // +slot?

        // TODO Create a ScreenQuad class or simular, since this is done in a few places!
        const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };
        // screen quad VAO and VBO
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

       
        //uint32_t RGB_FORMAT = GL_RGBA16F;
        //uint32_t RGB_FORMAT = GL_RGBA8;

        //uint32_t COLOR_FORMAT = GL_FLOAT;
        //uint32_t COLOR_FORMAT = GL_UNSIGNED_BYTE

      
        glGenFramebuffers(1, &m_BloomFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Bloom_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_Bloom_ColorAttachment);
        if (RGB_FORMAT == GL_RGBA16F)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Bloom_ColorAttachment, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            UX_LOG_ERROR("ERROR::Bloom_FRAMEBUFFER:: Framebuffer is not complete!");
        else UX_LOG_INFO("Bloom Framebuffer GOOD!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // framebuffer configuration
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

       
        
        if (m_Multisample)
        {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_BrightnessAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_BrightnessAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            

            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
            //glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, m_BrightnessAttachment, 0);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0); // REMOVE
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
        }
        else {
            // color attachment
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            // brightness color attachment
            glCreateTextures(GL_TEXTURE_2D, 1, &m_BrightnessAttachment);
            glBindTexture(GL_TEXTURE_2D, m_BrightnessAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            // depth attachment
            glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BrightnessAttachment, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
        }
       
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            UX_LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        else UX_LOG_INFO("Framebuffer GOOD!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ping-pong-framebuffer for blurring
        glGenFramebuffers(2, pingpongFBO);
        glCreateTextures(GL_TEXTURE_2D, 2, pingpongColorbuffers);

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            // also check if framebuffers are complete (no need for depth buffer)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // TODO ~Canvas() needs a lot of work!
    Canvas::~Canvas() = default;

    void Canvas::Resize(const uint32_t& width, const uint32_t& height)
    {
        m_Width = width;
        m_Height = height;

        glBindTexture(GL_TEXTURE_2D, m_Bloom_ColorAttachment);
        if (RGB_FORMAT == GL_RGBA16F)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (m_Multisample)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_BrightnessAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        }
        else {

        }

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Canvas::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glEnable(GL_DEPTH_TEST);
    }

    void Canvas::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
 
    void Canvas::Draw() const
    {
        // just copy into a non-multisampled texture
        
        m_Shader->Bind();
        m_Shader->SetUniform1i("u_ScreenTexture", 3); // slot
        m_Shader->SetUniform1i("u_Samples", m_Samples);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);
        glBindTextureUnit(3, m_BrightnessAttachment);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        

        // 2. blur bright fragments with two-pass Gaussian Blur 
        // --------------------------------------------------      
        bool horizontal = true, first_iteration = true;
        
        m_ShaderBlur->Bind();
        m_ShaderBlur->SetUniform1i("image", 3);
        m_ShaderBlur->SetUniform1i("u_Samples", m_Samples);
        for (auto i = 0; i < m_BlurAmount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            m_ShaderBlur->SetUniform1i("horizontal", horizontal);
            // bind texture of other framebuffer (or scene if first iteration)
            glBindTextureUnit(3, first_iteration ? m_Bloom_ColorAttachment : pingpongColorbuffers[!horizontal]);
            //renderQuad();
            // TODO Renderer::ScreenQuad();  DrawIndexed?
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to 
        // default framebuffer's (clamped) color range
        // ------------------------------------------------
        //Renderer::Clear();
        m_ShaderBloomFinal->Bind();
        m_ShaderBloomFinal->SetUniform1i("scene", 0);
        m_ShaderBloomFinal->SetUniform1i("bloomBlur", 1);
        m_ShaderBloomFinal->SetUniform1i("u_Samples", m_Samples);
        m_ShaderBloomFinal->SetUniform1i("bloom", m_Bloom);
        m_ShaderBloomFinal->SetUniform1f("exposure", m_Exposure);
        glBindTextureUnit(0, m_ColorAttachment);
        glBindTextureUnit(1, pingpongColorbuffers[!horizontal]);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDisable(GL_BLEND);
    }
}