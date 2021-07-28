#include <algine/core/font/FontMetrics.h>

#include <freetype/freetype.h>

#include <utility>
#include <codecvt>
#include <locale>

namespace algine {
struct FontData {
    FT_Glyph_Metrics metrics;
    char16_t c;
};

#define c_metrics (static_cast<FontData*>(m_data)->metrics)

FontMetrics::FontMetrics()
    : m_size(0), m_data(new FontData) {}

FontMetrics::FontMetrics(Font font, uint size)
    : m_font(std::move(font)),
      m_size(size),
      m_data(new FontData) {}

FontMetrics::FontMetrics(FontMetrics &&src) noexcept {
    src.swap(*this);
}

FontMetrics& FontMetrics::operator=(FontMetrics &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

FontMetrics::~FontMetrics() {
    delete (FontData*) m_data;
}

void FontMetrics::setFont(const Font &font) {
    m_font = font;
    static_cast<FontData*>(m_data)->c = 0;
}

void FontMetrics::setFont(const Font &font, uint size) {
    m_font = font;
    m_size = size;
    static_cast<FontData*>(m_data)->c = 0;
}

void FontMetrics::setFontSize(uint size) {
    m_size = size;
    static_cast<FontData*>(m_data)->c = 0;
}

const Font& FontMetrics::getFont() const {
    return m_font;
}

uint FontMetrics::getFontSize() const {
    return m_size;
}

int FontMetrics::width(char16_t c) {
    updateData(c);
    return c_metrics.width >> 6;
}

int FontMetrics::height(char16_t c) {
    updateData(c);
    return c_metrics.height >> 6;
}

int FontMetrics::horizontalAdvance(char16_t c) {
    updateData(c);
    return c_metrics.horiAdvance >> 6;
}

int FontMetrics::verticalAdvance(char16_t c) {
    updateData(c);
    return c_metrics.vertAdvance >> 6;
}

int FontMetrics::leftHorizontalBearing(char16_t c) {
    updateData(c);
    return c_metrics.horiBearingX >> 6;
}

int FontMetrics::topHorizontalBearing(char16_t c) {
    updateData(c);
    return c_metrics.horiBearingY >> 6;
}

int FontMetrics::leftVerticalBearing(char16_t c) {
    updateData(c);
    return c_metrics.vertBearingX >> 6;
}

int FontMetrics::topVerticalBearing(char16_t c) {
    updateData(c);
    return c_metrics.vertBearingY >> 6;
}

RectI FontMetrics::boundingRect(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utfConv;
    return boundingRect(utfConv.from_bytes(str));
}

RectI FontMetrics::boundingRect(const std::u16string &str) {
    if (str.empty()) {
        return {0, 0, 0, 0};
    }

    int x = 0;
    int y = 0;
    int minX = leftHorizontalBearing(str[0]);
    int minY = INT32_MAX;
    int maxY = INT32_MIN;

    for (auto c : str) {
        x += horizontalAdvance(c);

        int c_y0 = y + (height(c) - topHorizontalBearing(c));
        int c_y1 = c_y0 - height(c);

        if (c_y1 < minY) {
            minY = c_y1;
        }

        if (c_y0 > maxY) {
            maxY = c_y0;
        }
    }

    return {minX, minY, x - minX, maxY - minY};
}

bool FontMetrics::hasHorizontal() {
    return FT_HAS_HORIZONTAL(static_cast<FT_Face>(m_font.native_handle()));
}

bool FontMetrics::hasVertical() {
    return FT_HAS_VERTICAL(static_cast<FT_Face>(m_font.native_handle()));
}

void FontMetrics::swap(FontMetrics &src) {
    std::swap(m_font, src.m_font);
    std::swap(m_size, src.m_size);
    std::swap(m_data, src.m_data);
}

void FontMetrics::updateData(char16_t c) {
    auto data = static_cast<FontData*>(m_data);
    auto face = static_cast<FT_Face>(m_font.native_handle());

    if (data->c != c) {
        FT_Set_Pixel_Sizes(face, 0, m_size);
        FT_Load_Char(face, c, FT_LOAD_NO_BITMAP);

        data->c = c;
        data->metrics = face->glyph->metrics;
    }
}
}
