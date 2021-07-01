#ifndef ALGINE_FONTRENDERER_H
#define ALGINE_FONTRENDERER_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/font/Font.h>

#include <tulz/Array.h>

namespace algine {
class FontRenderer {
public:
    struct Character {
        tulz::Array<ubyte> data;
        uint width; // TODO: create class FontMetrics
        uint height;
        int bearingLeft;
        int bearingTop;
        long advance; // смещение до следующего глифа
    };

public:
    FontRenderer();
    FontRenderer(Font font, uint width, uint height);
    FontRenderer(Font font, uint height);

public:
    Character getCharacter(uint c) const;

    void setFontHeight(uint height);
    uint getFontHeight() const;

    void setFontWidth(uint width);
    uint getFontWidth() const;

    void setFontSize(uint width, uint height);
    void setFontSize(uint size);

    void setFont(const Font &font);
    const Font& getFont() const;

private:
    void applySize();

private:
    Font m_font;
    uint m_height;
    uint m_width;
};
}

#endif //ALGINE_FONTRENDERER_H
