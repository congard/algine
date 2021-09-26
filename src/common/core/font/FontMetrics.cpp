#include <algine/core/font/FontMetrics.h>

#include <freetype/freetype.h>

#include <utility>
#include <codecvt>
#include <locale>

#include "core/djb2.h"

namespace algine {
std::unordered_map<unsigned long, FontMetrics::Metrics> FontMetrics::m_metrics;

FontMetrics::FontMetrics()
    : m_size(0), m_metricsHash(0) {}

FontMetrics::FontMetrics(Font font, uint size)
    : m_font(std::move(font)),
      m_size(size),
      m_metricsHash(0)
{
    updateMetricsHash();
}

FontMetrics::FontMetrics(FontMetrics &&src) noexcept {
    src.swap(*this);
}

FontMetrics& FontMetrics::operator=(FontMetrics &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

FontMetrics::~FontMetrics() {
    disownMetricsHash();
}

void FontMetrics::setFont(const Font &font) {
    m_font = font;
    updateMetricsHash();
}

void FontMetrics::setFont(const Font &font, uint size) {
    m_font = font;
    m_size = size;
    updateMetricsHash();
}

void FontMetrics::setFontSize(uint size) {
    m_size = size;
    updateMetricsHash();
}

const Font& FontMetrics::getFont() const {
    return m_font;
}

uint FontMetrics::getFontSize() const {
    return m_size;
}

int FontMetrics::width(char16_t c) {
    return charMetrics(c).width;
}

int FontMetrics::height(char16_t c) {
    return charMetrics(c).height;
}

int FontMetrics::horizontalAdvance(char16_t c) {
    return charMetrics(c).horizontalAdvance;
}

int FontMetrics::verticalAdvance(char16_t c) {
    return charMetrics(c).verticalAdvance;
}

int FontMetrics::leftHorizontalBearing(char16_t c) {
    return charMetrics(c).leftHorizontalBearing;
}

int FontMetrics::topHorizontalBearing(char16_t c) {
    return charMetrics(c).topHorizontalBearing;
}

int FontMetrics::leftVerticalBearing(char16_t c) {
    return charMetrics(c).leftVerticalBearing;
}

int FontMetrics::topVerticalBearing(char16_t c) {
    return charMetrics(c).topVerticalBearing;
}

RectI FontMetrics::boundingRect(std::string_view str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utfConv;
    return boundingRect(utfConv.from_bytes(str.begin(), str.end()));
}

RectI FontMetrics::boundingRect(std::u16string_view str) {
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

    auto last = str.back();
    x -= horizontalAdvance(last); // horizontalAdvance = leftHorizontalBearing + width + extra
    x += leftHorizontalBearing(last) + width(last);

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
    std::swap(m_metricsHash, src.m_metricsHash);
}

void FontMetrics::updateMetricsHash() {
    if (!m_font.isLoaded()) {
        return;
    }

    std::string fontName = m_font.getName();
    std::string fontSize = std::to_string(m_size);
    std::string fontStyle = std::to_string(static_cast<uint>(m_font.getStyle()));

    disownMetricsHash();

    m_metricsHash = hash_djb2((unsigned char *) (fontName + " " + fontSize + " " + fontStyle).c_str());

    if (m_metrics.find(m_metricsHash) == m_metrics.end()) {
        m_metrics[m_metricsHash] = Metrics {};
    }

    m_metrics[m_metricsHash].counter++;
}

void FontMetrics::disownMetricsHash() {
    if (auto it = m_metrics.find(m_metricsHash); it != m_metrics.end()) {
        it->second.counter--;

        if (it->second.counter == 0) {
            m_metrics.erase(it);
        }
    }
}

FontMetrics::CharMetrics& FontMetrics::charMetrics(char16_t c) {
    auto &characters = m_metrics[m_metricsHash].characters;

    if (auto it = characters.find(c); it != characters.end()) {
        return it->second;
    } else {
        auto face = static_cast<FT_Face>(m_font.native_handle());

        FT_Set_Pixel_Sizes(face, 0, m_size);
        FT_Load_Char(face, c, FT_LOAD_NO_BITMAP);

        auto &rhs = face->glyph->metrics;
        CharMetrics lhs {};

        lhs.width = rhs.width >> 6;
        lhs.height = rhs.height >> 6;
        lhs.horizontalAdvance = rhs.horiAdvance >> 6;
        lhs.verticalAdvance = rhs.vertAdvance >> 6;
        lhs.leftHorizontalBearing = rhs.horiBearingX >> 6;
        lhs.topHorizontalBearing = rhs.horiBearingY >> 6;
        lhs.leftVerticalBearing = rhs.vertBearingX >> 6;
        lhs.topVerticalBearing = rhs.vertBearingY >> 6;

        return (characters[c] = lhs);
    }
}
}
