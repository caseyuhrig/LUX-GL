#pragma once

#include <pch.hpp>


namespace lux {

    enum class TextureFormat
    {
        None = 0,
        RGB = 1,
        RGBA = 2,
        Float16 = 3
    };

    enum class TextureWrap
    {
        None = 0,
        Clamp = 1,
        Repeat = 2
    };

    static constexpr GLenum ToOpenGLTextureFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGB:     return GL_RGB;
        case TextureFormat::RGBA:    return GL_RGBA;
        case TextureFormat::Float16: return GL_RGBA16F;
        }
        // TODO IMPLEMENT LUX_CORE_ASSERT(false, "Unknown texture format!");
        // there was something I saw on no macro logging! for c++
        // go find that :)
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////////////
    // Texture2D
    //////////////////////////////////////////////////////////////////////////////////

    class Texture2D
    {
    public:
        Texture2D(const std::string& path) : m_FilePath(path)
        {
            std::unique_ptr<unsigned char> buffer(LoadImage(path.c_str(), m_Width, m_Height, m_BPP, true));
            //auto* buffer = LoadImage(path.c_str(), m_Width, m_Height, m_BPP, true);
            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
            // these need to be specified!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());
            glBindTexture(GL_TEXTURE_2D, 0);
            // Free the image data loaded since it's been sent to openGl already.
            //if (buffer) stbi_image_free(buffer);
            if (buffer) buffer.reset();
        }
        ~Texture2D() { glDeleteTextures(1, &m_RendererID); }
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
        unsigned char* LoadImage(const std::string& path, T& width, T& height, T& bpp, const bool flipVertically)
        {
            int w, h, b;
            stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
            unsigned char* buffer = stbi_load(path.c_str(), &w, &h, &b, 4);
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