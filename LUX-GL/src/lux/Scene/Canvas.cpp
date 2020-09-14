#include "Canvas.hpp"

#include <glad/glad.h>
#include "../Renderer/Shader.hpp"
#include "../Log.hpp"


namespace lux {

    Canvas::Canvas() 
        : m_ColorAttachment(0), m_DepthAttachment(0), m_VAO(0), m_FBO(0), m_VBO(0), m_Width(0), m_Height(0) {}

    Canvas::Canvas(uint32_t width, uint32_t height, uint32_t samples)
    {
        Init(width, height, samples);
    }

    Canvas::~Canvas()
    {
    }

    void Canvas::Init(uint32_t width, uint32_t height, uint32_t samples)
    {
        m_Width = width;
        m_Height = height;
        m_Samples = samples;
        m_Multisample = m_Samples > 1;

        if (m_Multisample)
        {
            //m_ShaderBlur = lux::CreateRef<Shader>("res/shaders/shader-blur-MSAA.glsl");
            m_ShaderBloomFinal = lux::CreateRef<Shader>("res/shaders/shader-bloom_final-MSAA.glsl");
        }
        else {
            //m_ShaderBlur = lux::CreateRef<Shader>("res/shaders/shader-blur.glsl");
            m_ShaderBloomFinal = lux::CreateRef<Shader>("res/shaders/shader-bloom_final.glsl");
        }

        m_ShaderBlur = lux::CreateRef<Shader>("res/shaders/shader-blur.glsl");

        m_ShaderBlur->Bind();
        m_ShaderBlur->SetUniform1i("image", 0);
        m_ShaderBloomFinal->Bind();
        m_ShaderBloomFinal->SetUniform1i("scene", 0);
        m_ShaderBloomFinal->SetUniform1i("bloomBlur", 1);

        

        //if (m_Multisample)
            m_Shader = lux::CreateRef<Shader>("res/shaders/canvas-shader-MSAA.glsl");
        //else 
        //    m_Shader = lux::CreateRef<Shader>("res/shaders/canvas-shader.glsl");

        m_Shader->Bind();
        m_Shader->SetUniform1i("u_ScreenTexture", 0); // +slot?
        if (m_Multisample)
            m_Shader->SetUniform1i("u_Samples", m_Samples);

        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        uint32_t RGB_FORMAT = GL_RGBA16F;
        //uint32_t RGB_FORMAT = GL_RGBA8;

        // multisample framebuffer for output
        /*
        glGenFramebuffers(1, &m_MSAA_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_MSAA_FBO);
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_MSAA_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorAttachment);
        if (RGB_FORMAT == GL_RGBA16F)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
        else
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorAttachment, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            UX_LOG_ERROR("ERROR::MSAA_FRAMEBUFFER:: Framebuffer is not complete!");
        else UX_LOG_INFO("MSAA Framebuffer GOOD!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */
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
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
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
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        /* SAVING TO LOOK INTO MORE
        unsigned int fboMsaaId;
        if (MULTI_SAMPLE)
        {
            unsigned int rboC, rboD;
            glGenRenderbuffers(1, &rboC);
            glBindRenderbuffer(GL_RENDERBUFFER, rboC);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, window.GetWidth(), window.GetHeight());
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboC);

            glGenRenderbuffers(1, &rboD);
            glBindRenderbuffer(GL_RENDERBUFFER, rboD);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, window.GetWidth(), window.GetHeight());
            // GL_DEPTH_COMPONENT GL_DEPTH24_STENCIL8
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboD);


            glGenFramebuffers(1, &fboMsaaId);
            glBindFramebuffer(GL_FRAMEBUFFER, fboMsaaId);
            //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboC);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboD);
        }
        else {
            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        }
*/
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            UX_LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        else UX_LOG_INFO("Framebuffer GOOD!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //std::cout << "m_ColorAttachment: " << m_ColorAttachment << std::endl;


        // ping-pong-framebuffer for blurring
        //unsigned int pingpongFBO[2];
        //unsigned int pingpongColorbuffers[2];
        glGenFramebuffers(2, pingpongFBO);
        //glGenTextures(2, pingpongColorbuffers);
        //if (m_Multisample)
        //    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 2, pingpongColorbuffers);
        //else
            glCreateTextures(GL_TEXTURE_2D, 2, pingpongColorbuffers);

        for (unsigned int i = 0; i < 2; i++)
        {
            /*
            if (m_Multisample)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, pingpongColorbuffers[i]);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, pingpongColorbuffers[i], 0);
            }
            else {
            */
                glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            //}
            // also check if framebuffers are complete (no need for depth buffer)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Canvas::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        //renderer->Clear();
        glEnable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        //glDrawBuffers(2, attachments);
    }

    void Canvas::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
 
    void Canvas::Draw() const
    {
        m_ShaderBlur->Bind();
        m_ShaderBlur->SetUniform1i("image", 0);
        m_ShaderBloomFinal->Bind();
        m_ShaderBloomFinal->SetUniform1i("scene", 0);
        m_ShaderBloomFinal->SetUniform1i("bloomBlur", 1);

        //glEnable(GL_MULTISAMPLE);

        //m_Shader->Bind();     
        //glBindVertexArray(m_VAO);
        //glActiveTexture(GL_TEXTURE0); // + slot
        //if (m_Samples > 1) glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
        //else glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        
       //glActiveTexture(GL_TEXTURE0);

        /*
        glActiveTexture(GL_TEXTURE0);
        if (m_Samples > 1) glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_BrightnessAttachment);
        else glBindTexture(GL_TEXTURE_2D, m_BrightnessAttachment);	// use the color attachment texture as the texture of the quad plane
        */

        // just copy into a non-multisampled texture
        glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_BrightnessAttachment);
        m_Shader->Bind();
        m_Shader->SetUniform1i("u_ScreenTexture", 3); // +slot?
        m_Shader->SetUniform1i("u_Samples", 8); //m_Samples);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);



        //glActiveTexture(GL_TEXTURE0);
        // 2. blur bright fragments with two-pass Gaussian Blur 
        // --------------------------------------------------
        
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_Bloom_ColorAttachment);

        m_ShaderBlur->Bind();
        m_ShaderBlur->SetUniform1i("image", 3);
        //m_ShaderBlur->SetUniform1i("u_Samples", m_Samples);
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            m_ShaderBlur->SetUniform1i("horizontal", horizontal);
           // if (m_Multisample)
             //   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, first_iteration ? m_BrightnessAttachment : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
           // else
                glBindTexture(GL_TEXTURE_2D, first_iteration ? m_Bloom_ColorAttachment : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            //renderQuad();
            glBindVertexArray(m_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //return;


        // MSAA BEGIN
        //glBindFramebuffer(GL_FRAMEBUFFER, m_MSAA_FBO);
        //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorAttachment);
        //glEnable(GL_DEPTH_TEST);


        // 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_ShaderBloomFinal->Bind();
        if (m_Multisample) 
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, pingpongColorbuffers[!horizontal]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        }
        m_ShaderBloomFinal->SetUniform1i("u_Samples", m_Samples);
        m_ShaderBloomFinal->SetUniform1i("bloom", m_Bloom);
        m_ShaderBloomFinal->SetUniform1f("exposure", m_Exposure);

        //glActiveTexture(GL_TEXTURE2);
        //glEnable(GL_MULTISAMPLE);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisable(GL_BLEND);

        // NOW RENDER TO THE DEFAULT FB
        /*
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSAA_ColorAttachment);

        m_Shader->Bind();
        m_Shader->SetUniform1i("u_ScreenTexture", 2); // +slot?
        m_Shader->SetUniform1i("u_Samples", 8); //m_Samples);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        */
    }
}