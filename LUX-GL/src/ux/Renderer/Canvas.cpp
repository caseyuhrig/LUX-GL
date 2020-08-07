#include "Canvas.hpp"

#include <glad/glad.h>
#include "../../Shader.hpp"
#include "../../ux/Log.hpp"


namespace lux {

    Canvas::Canvas() 
        : m_ColorAttachment(0), m_DepthAttachment(0), m_VAO(0), m_FBO(0), m_VBO(0), m_Width(0), m_Height(0)
    {
        m_Shader = ux::CreateRef<Shader>("res/shaders/framebuffer.shader");
    }

    Canvas::~Canvas() 
    {
    }

    void Canvas::Init(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;

        //auto frameBufferShader = Shader("res/shaders/framebuffer.shader");
        m_Shader->Bind();
        m_Shader->SetUniform1i("screenTexture", 0);

        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };
        // screen quad VAO
        //unsigned int quadVAO;
        //unsigned int quadVBO;
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
  // -------------------------
        //unsigned int framebuffer;
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        // create a color attachment texture
        //unsigned int textureColorbuffer, m_DepthAttachment;
        //glGenTextures(1, &textureColorbuffer);  
        if (m_Samples > 1)
        {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGBA8, m_Width, m_Height, GL_FALSE);
            //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);


            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0); // REMOVE
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
        }
        else {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // GL_RGBA?
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

            glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
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
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            UX_LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        else UX_LOG_DEBUG("FB GOOD!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        std::cout << "m_ColorAttachment: " << m_ColorAttachment << std::endl;
    }


    void Canvas::Begin()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void Canvas::End()
    {
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Canvas::Render() const
    {
        // frameBuffer.Init(); canvas.Init();
          // framebuffer.Bind(); canvas.Bind();
          // renderer.Clear();
          // <draw>
          // frameBuffer.UnBind(); canvas.UnBind();
          // renderer.Clear();
          // renderer.DisableDepthTest();
          // frameBuffer.Draw();    canvas.Draw();

          // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_Shader->Bind();
        //m_Shader.SetUniform1i("screenTexture", textureColorbuffer);
        glActiveTexture(GL_TEXTURE0); // + slot WOW guy didn't do this learnopengl         
        glBindVertexArray(m_VAO);
        if (m_Samples > 1) glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
        else glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}