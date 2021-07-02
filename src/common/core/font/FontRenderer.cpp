#include <algine/core/font/FontRenderer.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <utility>

#define face static_cast<FT_Face>(m_font.native_handle())

namespace algine {
FontRenderer::FontRenderer()
    : m_width(0), m_height(0) {}

FontRenderer::FontRenderer(Font font, uint width, uint height)
    : m_font(std::move(font)),
      m_width(width), m_height(height) {}

FontRenderer::FontRenderer(Font font, uint height)
    : m_font(std::move(font)),
      m_width(0), m_height(height) {}

void FontRenderer::setFontHeight(uint height) {
    m_height = height;
}

uint FontRenderer::getFontHeight() const {
    return m_height;
}

void FontRenderer::setFontWidth(uint width) {
    m_width = width;
}

uint FontRenderer::getFontWidth() const {
    return m_width;
}

void FontRenderer::setFontSize(uint width, uint height) {
    m_height = height;
    m_width = width;
}

void FontRenderer::setFontSize(uint size) {
    setFontHeight(size);
}

void FontRenderer::setFont(const Font &font) {
    m_font = font;
}

const Font& FontRenderer::getFont() const {
    return m_font;
}

FontRenderer::Character FontRenderer::getCharacter(uint c) const {
    FT_Set_Pixel_Sizes(face, m_width, m_height);
    FT_Load_Char(face, c, FT_LOAD_RENDER);

    Character character;
    character.width = face->glyph->bitmap.width;
    character.height = face->glyph->bitmap.rows;
    character.bearingLeft = face->glyph->bitmap_left;
    character.bearingTop = face->glyph->bitmap_top;
    character.advance = face->glyph->advance.x;
    character.data = tulz::Array<ubyte>(character.width * character.height);

    auto buffer = face->glyph->bitmap.buffer;

    for (uint i = 0; i < character.data.size(); i++) {
        character.data[i] = buffer[i];
    }

    return character;
}
}
