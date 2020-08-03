#pragma once

//#include <cstdint>
//#include <memory>
#include "ux/ux.h"

// http://www.lighthouse3d.com/tutorials/opengl-selection-tutorial/

struct FrameBufferSpecification
{
    unsigned int Width, Height;
    unsigned int Samples = 1;
	bool SwapChainTarget = false;
};


class FrameBuffer
{
private:
    unsigned int _fboID;
    unsigned int _ColorAttachment;
    unsigned int _DepthAttachment;
    FrameBufferSpecification _Specification;
public:
    static const unsigned int MaxFramebufferSize = 8192;
    
    inline FrameBuffer(FrameBufferSpecification spec) 
        : _fboID(0), _Specification(spec), _ColorAttachment(0), _DepthAttachment(0)
    {
        Invalidate();
    }

    ~FrameBuffer()
    {
        glDeleteFramebuffers(1, &_fboID);
        glDeleteTextures(1, &_ColorAttachment);
        glDeleteTextures(1, &_DepthAttachment);
    }

    inline ux::Ref<FrameBuffer> Create(const FrameBufferSpecification& spec)
    {
        return ux::CreateRef<FrameBuffer>(spec);
    }

    const FrameBufferSpecification& GetSpecification() const {
        return _Specification;
    }

	unsigned int GetColorAttachmentID() const { return _ColorAttachment; }

	// TODO make resize
	void Invalidate()
	{
		if (_fboID)
		{
			glDeleteFramebuffers(1, &_fboID);
			glDeleteTextures(1, &_ColorAttachment);
			glDeleteTextures(1, &_DepthAttachment);
		}

		glCreateFramebuffers(1, &_fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, _fboID);

		glCreateTextures(GL_TEXTURE_2D, 1, &_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, _ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _Specification.Width, _Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, _DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, _Specification.Width, _Specification.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _DepthAttachment, 0);

		// TODO needs to be in an assert
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			UX_LOG_ERROR("Framebuffer is incomplete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
		glViewport(0, 0, _Specification.Width, _Specification.Height);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
		{
			// TODO make warn
			UX_LOG_INFO("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		_Specification.Width = width;
		_Specification.Height = height;

		Invalidate();
	}

};