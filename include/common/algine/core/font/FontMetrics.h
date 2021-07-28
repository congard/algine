#ifndef ALGINE_FONTMETRICS_H
#define ALGINE_FONTMETRICS_H

#include <algine/core/font/Font.h>
#include <algine/core/Rect.h>

namespace algine {
class FontMetrics {
public:
    FontMetrics();
    FontMetrics(Font font, uint size);
    FontMetrics(FontMetrics &&src) noexcept;
    FontMetrics& operator=(FontMetrics &&rhs) noexcept;
    ~FontMetrics();

    void setFont(const Font &font);
    void setFont(const Font &font, uint size);
    void setFontSize(uint size);

    const Font& getFont() const;
    uint getFontSize() const;

    int width(char16_t c);
    int height(char16_t c);
    int horizontalAdvance(char16_t c);
    int verticalAdvance(char16_t c);
    int leftHorizontalBearing(char16_t c);
    int topHorizontalBearing(char16_t c);
    int leftVerticalBearing(char16_t c);
    int topVerticalBearing(char16_t c);

    RectI boundingRect(const std::string &str);
    RectI boundingRect(const std::u16string &str);

    bool hasHorizontal();
    bool hasVertical();

    void swap(FontMetrics &src);

private:
    FontMetrics(const FontMetrics&);
    FontMetrics& operator=(const FontMetrics&);

    void updateData(char16_t c);

private:
    Font m_font;
    uint m_size;
    void* m_data;
};
}

#endif //ALGINE_FONTMETRICS_H
