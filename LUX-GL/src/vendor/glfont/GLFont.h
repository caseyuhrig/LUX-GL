#pragma once

#include <gl\glew.h>
#include "ft2build.h"
#include FT_FREETYPE_H

#include <string>


class GLFont {
public:
    GLFont(const std::string& fontFile);
    ~GLFont() = default;
    void SetFontFile(const std::string& fontFile);
    const FT_Face& GetFontFace() const noexcept { return m_FontFace; }
private:
    FT_Library m_FreeType;
    FT_Face m_FontFace;
    std::string m_FontFile;
};

