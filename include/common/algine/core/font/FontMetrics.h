#ifndef ALGINE_FONTMETRICS_H
#define ALGINE_FONTMETRICS_H

#include <algine/core/font/Font.h>
#include <algine/core/Rect.h>
#include <algine/types.h>

#include <unordered_map>

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

    RectI boundingRect(std::string_view str);
    RectI boundingRect(std::u16string_view str);

    bool hasHorizontal();
    bool hasVertical();

    void swap(FontMetrics &src);

private:
    FontMetrics(const FontMetrics&);
    FontMetrics& operator=(const FontMetrics&);

    void updateMetricsHash();
    void disownMetricsHash();

private:
    Font m_font;
    uint m_size;

    unsigned long m_metricsHash;

private:
    struct CharMetrics {
        int width;
        int height;
        int horizontalAdvance;
        int verticalAdvance;
        int leftHorizontalBearing;
        int topHorizontalBearing;
        int leftVerticalBearing;
        int topVerticalBearing;
    };

    struct Metrics {
        std::unordered_map<char16_t, CharMetrics> characters;
        int counter {0};
    };

    static std::unordered_map<unsigned long, Metrics> m_metrics;

    CharMetrics& charMetrics(char16_t c);
};
}

#endif //ALGINE_FONTMETRICS_H
