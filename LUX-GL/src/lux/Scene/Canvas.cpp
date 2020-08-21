#include "Canvas.hpp"

#include <glad/glad.h>
#include "../Renderer/Shader.hpp"
#include "../Log.hpp"


namespace lux {

    Canvas::Canvas() 
        : m_ColorAttachment(0), m_DepthAttachment(0), m_VAO(0), m_FBO(0), m_VBO(0), m_Width(0), m_Height(0)
    {
        
    }

    Canvas::~Canvas() 
    {
    }

    void Canvas::Init(uint32_t width, uint32_t height, uint32_t samples)
    {
        m_Width = width;
        m_Height = height;
        m_Samples = samples;
        bool multisample = m_Samples > 1;

        if (multisample) 
            m_Shader = lux::CreateRef<Shader>("res/shaders/canvas-shader-MSAA.glsl");
        else 
            m_Shader = lux::CreateRef<Shader>("res/shaders/canvas-shader.glsl");

        m_Shader->Bind();
        m_Shader->SetUniform1i("u_ScreenTexture", 0); // +slot?
        if (multisample)
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

        // framebuffer configuration
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        uint32_t RGB_FORMAT = GL_RGBA16F;
        //uint32_t RGB_FORMAT = GL_RGBA8;
        
        if (multisample)
        {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            /*
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_BrightnessAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            if (RGB_FORMAT == GL_RGBA16F)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA16F, m_Width, m_Height, GL_FALSE);
            else
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            */

            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
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
        //renderer->Clear();
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // disable blending, does not work with HDR! RGBA16F
        //glDisable(GL_BLEND);
        m_Shader->Bind();     
        glBindVertexArray(m_VAO);
        glActiveTexture(GL_TEXTURE0); // + slot
        if (m_Samples > 1) glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
        else glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}