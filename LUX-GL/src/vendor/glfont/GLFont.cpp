#include "GLFont.h"

#include <string>
#include <exception>


GLFont::GLFont(const std::string& fontFile)
{
    const FT_Error _error = FT_Init_FreeType(&m_FreeType);
    if (_error) {
        throw std::exception("Failed to initialize FreeType");
    }
    SetFontFile(fontFile);
}


void GLFont::SetFontFile(const std::string& fontFile)
{
    m_FontFile = fontFile;
    const FT_Error _error = FT_New_Face(m_FreeType, m_FontFile.c_str(), 0, &m_FontFace);

    if (_error == FT_Err_Unknown_File_Format)
        throw std::exception("Failed to open font: unknown font format");
    else if (_error)
        throw std::exception("Failed to open font");
}