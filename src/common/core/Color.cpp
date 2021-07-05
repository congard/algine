#include <algine/core/Color.h>

namespace algine {
Color::Color()
    : m_red(0), m_green(0), m_blue(0), m_alpha(0) {}

Color::Color(uint color)
    : m_red((color >> 16) & 0xff),
      m_green((color >> 8) & 0xff),
      m_blue(color & 0xff),
      m_alpha((color >> 24) & 0xff) {}

Color::Color(int r, int g, int b, int a)
    : m_red(r), m_green(g), m_blue(b), m_alpha(a) {}

int Color::red() const {
    return m_red;
}

int Color::green() const {
    return m_green;
}

int Color::blue() const {
    return m_blue;
}

int Color::alpha() const {
    return m_alpha;
}

float Color::redF() const {
    return static_cast<float>(m_red) / 255.0f;
}

float Color::greenF() const {
    return static_cast<float>(m_green) / 255.0f;
}

float Color::blueF() const {
    return static_cast<float>(m_blue) / 255.0f;
}

float Color::alphaF() const {
    return static_cast<float>(m_alpha) / 255.0f;
}

uint Color::value() const {
    return (m_alpha & 0xff) << 24 | (m_red & 0xff) << 16 | (m_green & 0xff) << 8 | (m_blue & 0xff);
}

void Color::setValue(uint color) {
    m_red = (color >> 16) & 0xff;
    m_green = (color >> 8) & 0xff;
    m_blue = color & 0xff;
    m_alpha = (color >> 24) & 0xff;
}

void Color::setRgb(int r, int g, int b, int a) {
    m_red = r;
    m_green = g;
    m_blue = b;
    m_alpha = a;
}

void Color::setRgbF(float r, float g, float b, float a) {
    m_red = static_cast<int>(r * 255);
    m_green = static_cast<int>(g * 255);
    m_blue = static_cast<int>(b * 255);
    m_alpha = static_cast<int>(a * 255);
}

Color Color::fromRgbF(float r, float g, float b, float a) {
    Color color;
    color.setRgbF(r, g, b, a);
    return color;
}
}
