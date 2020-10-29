#pragma once

#include <glad/glad.h>
#include <string>

#include "stb_image/stb_image.h"


namespace lux {

    class Texture
    {
    public:

        Texture(const std::string& path) : m_FilePath(path)
        {
            void* buffer = LoadImage(path.c_str(), m_Width, m_Height, m_BPP, true);
            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
            // these need to be specified!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            glBindTexture(GL_TEXTURE_2D, 0);
            // Free the image data loaded since it's been sent to openGl already.
            if (buffer) stbi_image_free(buffer);
        }
        ~Texture() { glDeleteTextures(1, &m_RendererID); }
        void Bind(uint32_t slot /* = 0*/) const
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
        }
        void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
    protected:
        template<typename T>
        void* LoadImage(const std::string& path, T& width, T& height, T& bpp, const bool flipVertically)
        {
            int w, h, b;
            stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
            const void* buffer = stbi_load(path.c_str(), &w, &h, &b, 4);
            width = static_cast<T>(w);
            height = static_cast<T>(h);
            bpp = static_cast<T>(b);
            return buffer;
        }
    private:
        uint32_t m_RendererID;
        std::string m_FilePath;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_BPP;
    };
}