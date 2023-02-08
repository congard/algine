#include <algine/core/Color.h>

#include <sstream>
#include <cmath>
#include <glm/common.hpp>

#include "hsluv/hsluv.h"

namespace algine {
constexpr ushort USHORT_MAX = -1;

/**
 * ubyte range => ushort range
 */
template<typename T>
inline ushort m257(T p) {
    return p * 257;
}

/**
 * ushort range => ubyte range
 */
inline ubyte d257(ushort p) {
    return static_cast<ubyte>(round(p / 257.0));
}

/**
 * ushort range => float [0, 1] range
 */
inline float frange(ushort p) {
    return static_cast<float>(p) / static_cast<float>(USHORT_MAX);
}

/**
 * float [0, 1] range => ushort range
 */
inline ushort usrange(float p) {
    return static_cast<ushort>(p * USHORT_MAX);
}

/**
 * degrees [0, 360] => ushort [0, 65535]
 */
inline ushort deg2us(float deg) {
    return static_cast<ushort>((float) USHORT_MAX / 360.0f * deg);
}

/**
 * ushort [0, 65535] => degrees [0, 360]
 */
inline float us2deg(ushort us) {
    return (float) us / (float) USHORT_MAX * 360.0f;
}

/**
 * @tparam T
 * @param a
 * @param lower lower bound
 * @param upper upper bound
 * @return position (i.e. value in range [0, 1]) for parameter `a`
 */
template<typename T>
inline T posBetween(T a, T lower, T upper) {
    return (a - lower) / (upper - lower);
}

template<typename T>
inline float posBetweenF(T a, T lower, T upper) {
    return posBetween(a, lower, upper);
}

constexpr struct {
    double l_min = 0.0, l_max = 100.0;
    double u_min = -83.067120, u_max = 175.009822;
    double v_min = -134.109878, v_max = 107.418111;
} luvInfo;

constexpr struct {
    double l_min = 0.0, l_max = 100.0;
    double c_min = 0.0, c_max = 179.038097;
    double h_min = 0.0, h_max = 360.0;
} lchInfo;

template<typename T, typename ...Args>
void assert_values(T from, T to, T val, Args... values) {
    if (!(from <= val && val <= to)) {
        throw std::invalid_argument(
            "Color value out of range:\n"
            "\texpected: " + std::to_string(from) + " <= x <= " + std::to_string(to) +
            "\tactual: x = " + std::to_string(val)
        );
    }

    if constexpr (sizeof...(values) > 0) {
        assert_values(from, to, values...);
    }
}

template<typename ...Args>
void assert_0_255(int val, Args... values) {
    assert_values(0, 255, val, values...);
}

template<typename ...Args>
void assert_0_1(float val, Args... values) {
    assert_values(0.0f, 1.0f, val, values...);
}

template<typename ...Args>
void assert_deg(int val, Args... values) {
    assert_values(0, 360, val, values...);
}

#define assert_rgb assert_0_255
#define assert_rgbf assert_0_1

Color::Color()
    : m_sp(0, 0, 0, 0) {}

Color::Color(uint color)
    : Color(
            (color >> 16) & 0xff,
            (color >> 8) & 0xff,
            color & 0xff,
            (color >> 24) & 0xff
      ) {}

Color::Color(int r, int g, int b, int a)
    : m_sp(m257(r), m257(g), m257(b), m257(a)),
      m_space(Space::Rgb)
{
    assert_rgb(r, g, b, a);
}

bool Color::operator==(const Color &rhs) const {
    return getRgb() == rhs.getRgb() && alpha() == rhs.alpha();
}

bool Color::operator!=(const Color &rhs) const {
    return !(*this == rhs);
}

int Color::red() const {
    if (m_space != Space::Rgb)
        return toRgb().red();
    return d257(m_sp.rgb.r);
}

int Color::green() const {
    if (m_space != Space::Rgb)
        return toRgb().green();
    return d257(m_sp.rgb.g);
}

int Color::blue() const {
    if (m_space != Space::Rgb)
        return toRgb().blue();
    return d257(m_sp.rgb.b);
}

int Color::alpha() const {
    return d257(m_sp.rgb.a);
}

float Color::redF() const {
    if (m_space != Space::Rgb)
        return toRgb().redF();
    return frange(m_sp.rgb.r);
}

float Color::greenF() const {
    if (m_space != Space::Rgb)
        return toRgb().greenF();
    return frange(m_sp.rgb.g);
}

float Color::blueF() const {
    if (m_space != Space::Rgb)
        return toRgb().blueF();
    return frange(m_sp.rgb.b);
}

float Color::alphaF() const {
    return frange(m_sp.rgb.a);
}

uint Color::value() const {
    if (m_space != Space::Rgb)
        return toRgb().value();

    const auto &c = m_sp.rgb;
    uint a = d257(c.a);
    uint r = d257(c.r);
    uint g = d257(c.g);
    uint b = d257(c.b);

    return (a & 0xff) << 24 | (r & 0xff) << 16 | (g & 0xff) << 8 | (b & 0xff);
}

bool Color::isValid() const {
    return m_space != Space::Invalid;
}

void Color::setValue(uint color) {
    setRgb(
        (color >> 16) & 0xff,
        (color >> 8) & 0xff,
        color & 0xff,
        (color >> 24) & 0xff
    );
}

void Color::setRgb(int r, int g, int b, int a) {
    assert_rgb(r, g, b, a);
    m_space = Space::Rgb;
    auto &c = m_sp.rgb;
    c.r = m257(r);
    c.g = m257(g);
    c.b = m257(b);
    c.a = m257(a);
}

void Color::setRgbF(float r, float g, float b, float a) {
    assert_rgbf(r, g, b, a);
    m_space = Space::Rgb;
    auto &c = m_sp.rgb;
    c.r = usrange(r);
    c.g = usrange(g);
    c.b = usrange(b);
    c.a = usrange(a);
}

void Color::setRed(int red) {
    assert_rgb(red);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgb(red, rgb.green(), rgb.blue(), rgb.alpha());
    } else {
        m_sp.rgb.r = m257(red);
    }
}

void Color::setGreen(int green) {
    assert_rgb(green);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgb(rgb.red(), green, rgb.blue(), rgb.alpha());
    } else {
        m_sp.rgb.g = m257(green);
    }
}

void Color::setBlue(int blue) {
    assert_rgb(blue);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgb(rgb.red(), rgb.green(), blue, rgb.alpha());
    } else {
        m_sp.rgb.b = m257(blue);
    }
}

void Color::setAlpha(int alpha) {
    assert_rgb(alpha);
    m_sp.rgb.a = m257(alpha);
}

void Color::setRedF(float red) {
    assert_rgbf(red);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgbF(red, rgb.greenF(), rgb.blueF(), rgb.alphaF());
    } else {
        m_sp.rgb.r = usrange(red);
    }
}

void Color::setGreenF(float green) {
    assert_rgbf(green);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgbF(rgb.redF(), green, rgb.blueF(), rgb.alphaF());
    } else {
        m_sp.rgb.g = usrange(green);
    }
}

void Color::setBlueF(float blue) {
    assert_rgbf(blue);

    if (m_space != Space::Rgb) {
        auto rgb = toRgb();
        setRgbF(rgb.redF(), rgb.greenF(), blue, rgb.alphaF());
    } else {
        m_sp.rgb.b = usrange(blue);
    }
}

void Color::setAlphaF(float alpha) {
    assert_rgbf(alpha);
    m_sp.rgb.a = usrange(alpha);
}

Color Color::hsvHueRotate(int degrees) const {
    return hsvHueRotateF(static_cast<float>(degrees) / 360.0f);
}

Color Color::hsvHueRotateF(float val) const {
    auto hsv = getHsvF();
    hsv[0] = fmodf(hsv[0] + val, 1.0f);
    return Color::fromHsvF(hsv[0], hsv[1], hsv[2], alphaF());
}

Color Color::lchHueRotate(int degrees) const {
    return lchHueRotateF(static_cast<float>(degrees) / 360.0f);
}

Color Color::lchHueRotateF(float val) const {
    auto lch = getLchF();
    lch[2] = fmodf(lch[2] + val, 1.0f);
    return Color::fromLchF(lch[0], lch[1], lch[2], alphaF());
}

Color Color::hsluvHueRotate(int degrees) const {
    return hsluvHueRotateF(static_cast<float>(degrees) / 360.0f);
}

Color Color::hsluvHueRotateF(float val) const {
    auto hsluv = getHsluvF();
    hsluv[0] = fmodf(hsluv[0] + val, 1.0f);
    return Color::fromHsluvF(hsluv[0], hsluv[1], hsluv[2], alphaF());
}

glm::ivec3 Color::getRgb() const {
    if (m_space != Space::Rgb)
        return toRgb().getRgb();
    return {red(), green(), blue()};
}

glm::vec3 Color::getRgbF() const {
    if (m_space != Space::Rgb)
        return toRgb().getRgbF();
    return {redF(), greenF(), blueF()};
}

glm::ivec3 Color::getHsv() const {
    if (m_space != Space::Hsv)
        return toHsv().getHsv();
    return {
        static_cast<int>(us2deg(m_sp.hsv.h)),
        d257(m_sp.hsv.s),
        d257(m_sp.hsv.v)
    };
}

glm::vec3 Color::getHsvF() const {
    if (m_space != Space::Hsv)
        return toHsv().getHsvF();
    return {
        frange(m_sp.hsv.h),
        frange(m_sp.hsv.s),
        frange(m_sp.hsv.v)
    };
}

glm::ivec3 Color::getLuv() const {
    if (m_space != Space::Luv)
        return toLuv().getLuv();
    return {
        d257(m_sp.luv.l),
        d257(m_sp.luv.u),
        d257(m_sp.luv.v)
    };
}

glm::vec3 Color::getLuvF() const {
    if (m_space != Space::Luv)
        return toLuv().getLuvF();
    return {
        frange(m_sp.luv.l),
        frange(m_sp.luv.u),
        frange(m_sp.luv.v)
    };
}

glm::ivec3 Color::getLch() const {
    if (m_space != Space::Lch)
        return toLch().getLch();
    return {
        d257(m_sp.lch.l),
        d257(m_sp.lch.c),
        static_cast<int>(us2deg(m_sp.lch.h))
    };
}

glm::vec3 Color::getLchF() const {
    if (m_space != Space::Lch)
        return toLch().getLchF();
    return {
        frange(m_sp.lch.l),
        frange(m_sp.lch.c),
        frange(m_sp.lch.h)
    };
}

glm::ivec3 Color::getHsluv() const {
    if (m_space != Space::Hsluv)
        return toHsluv().getHsluv();
    return {
        static_cast<int>(us2deg(m_sp.hsluv.h)),
        d257(m_sp.hsluv.s),
        d257(m_sp.hsluv.l)
    };
}

glm::vec3 Color::getHsluvF() const {
    if (m_space != Space::Hsluv)
        return toHsluv().getHsluvF();
    return {
        frange(m_sp.hsluv.h),
        frange(m_sp.hsluv.s),
        frange(m_sp.hsluv.l)
    };
}

void Color::setHsv(int h, int s, int v, int a) {
    assert_deg(h);
    assert_0_255(s, v, a);
    m_space = Space::Hsv;
    auto &hsv = m_sp.hsv;
    hsv.h = deg2us(static_cast<float>(h));
    hsv.s = m257(s);
    hsv.v = m257(v);
    hsv.a = m257(a);
}

void Color::setHsvF(float h, float s, float v, float a) {
    assert_0_1(h, s, v, a);
    m_space = Space::Hsv;
    auto &hsv = m_sp.hsv;
    hsv.h = usrange(h);
    hsv.s = usrange(s);
    hsv.v = usrange(v);
    hsv.a = usrange(a);
}

void Color::setLuv(int l, int u, int v, int a) {
    assert_0_255(l, u, v, a);
    m_space = Space::Luv;
    auto &luv = m_sp.luv;
    luv.l = m257(l);
    luv.u = m257(u);
    luv.v = m257(v);
    luv.a = m257(a);
}

void Color::setLuvF(float l, float u, float v, float a) {
    assert_0_1(l, u, v, a);
    m_space = Space::Luv;
    auto &luv = m_sp.luv;
    luv.l = usrange(l);
    luv.u = usrange(u);
    luv.v = usrange(v);
    luv.a = usrange(a);
}

void Color::setLch(int l, int c, int h, int a) {
    assert_deg(h);
    assert_0_255(l, c, a);
    m_space = Space::Lch;
    auto &lch = m_sp.lch;
    lch.l = m257(l);
    lch.c = m257(c);
    lch.h = deg2us(static_cast<float>(h));
    lch.a = m257(a);
}

void Color::setLchF(float l, float c, float h, float a) {
    assert_0_1(l, c, h, a);
    m_space = Space::Lch;
    auto &lch = m_sp.lch;
    lch.l = usrange(l);
    lch.c = usrange(c);
    lch.h = usrange(h);
    lch.a = usrange(a);
}

void Color::setHsluv(int h, int s, int l, int a) {
    assert_deg(h);
    assert_0_255(s, l, a);
    m_space = Space::Hsluv;
    auto &hsluv = m_sp.hsluv;
    hsluv.h = deg2us(static_cast<float>(h));
    hsluv.s = m257(s);
    hsluv.l = m257(l);
    hsluv.a = m257(a);
}

void Color::setHsluvF(float h, float s, float l, float a) {
    assert_0_1(h, s, l, a);
    m_space = Space::Hsluv;
    auto &hsluv = m_sp.hsluv;
    hsluv.h = usrange(h);
    hsluv.s = usrange(s);
    hsluv.l = usrange(l);
    hsluv.a = usrange(a);
}

std::string Color::toString() const {
    std::stringstream stream;
    stream << std::hex << value();

    std::string str = stream.str();
    auto result = str.c_str();

    // if alpha is 255, exclude it from result
    if (alpha() == 255) {
        result += 2; // ff
    }

    return "#" + std::string(result);
}

Color::Space Color::getSpace() const {
    return m_space;
}

// RGB <=> HSV source:
// https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72

/*! \brief Convert RGB to HSV color space

  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0, 1], respectively.

  \param r Red component, used as input, range: [0, 1]
  \param g Green component, used as input, range: [0, 1]
  \param b Blue component, used as input, range: [0, 1]
  \param h Hue component, used as output, range: [0, 360]
  \param s Hue component, used as output, range: [0, 1]
  \param v Hue component, used as output, range: [0, 1]

*/
void rgb2hsv(float r, float g, float b, float &h, float &s, float &v) {
    float cMax = std::max(std::max(r, g), b);
    float cMin = std::min(std::min(r, g), b);
    float delta = cMax - cMin;

    if (delta > 0) {
        if (cMax == r) {
            h = 60 * fmodf(((g - b) / delta), 6);
        } else if (cMax == g) {
            h = 60 * (((b - r) / delta) + 2);
        } else if (cMax == b) {
            h = 60 * (((r - g) / delta) + 4);
        }

        if (cMax > 0) {
            s = delta / cMax;
        } else {
            s = 0;
        }

        v = cMax;
    } else {
        h = 0;
        s = 0;
        v = cMax;
    }

    if (h < 0) {
        h = 360 + h;
    }
}

/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param r Red component, used as output, range: [0, 1]
  \param g Green component, used as output, range: [0, 1]
  \param b Blue component, used as output, range: [0, 1]
  \param h Hue component, used as input, range: [0, 360]
  \param s Hue component, used as input, range: [0, 1]
  \param v Hue component, used as input, range: [0, 1]

*/
void hsv2rgb(float &r, float &g, float &b, float h, float s, float v) {
    float c = v * s; // Chroma
    float hPrime = fmodf(h / 60.0f, 6);
    float x = c * (1 - fabsf(fmodf(hPrime, 2) - 1));
    float m = v - c;

    if (0 <= hPrime && hPrime < 1) {
        r = c;
        g = x;
        b = 0;
    } else if (1 <= hPrime && hPrime < 2) {
        r = x;
        g = c;
        b = 0;
    } else if (2 <= hPrime && hPrime < 3) {
        r = 0;
        g = c;
        b = x;
    } else if (3 <= hPrime && hPrime < 4) {
        r = 0;
        g = x;
        b = c;
    } else if (4 <= hPrime && hPrime < 5) {
        r = x;
        g = 0;
        b = c;
    } else if (5 <= hPrime && hPrime < 6) {
        r = c;
        g = 0;
        b = x;
    } else {
        r = 0;
        g = 0;
        b = 0;
    }

    r += m;
    g += m;
    b += m;
}

Color Color::toRgb() const {
    switch (m_space) {
        case Space::Invalid: throw std::runtime_error("Invalid color");
        case Space::Rgb: return *this;
        case Space::Hsv: {
            float r, g, b;
            hsv2rgb(r, g, b, frange(m_sp.hsv.h) * 360.0f, frange(m_sp.hsv.s), frange(m_sp.hsv.v));
            return Color::fromRgbF(r, g, b, alphaF());
        }
        case Space::Luv: {
            double l = glm::mix(luvInfo.l_min, luvInfo.l_max, frange(m_sp.luv.l));
            double u = glm::mix(luvInfo.u_min, luvInfo.u_max, frange(m_sp.luv.u));
            double v = glm::mix(luvInfo.v_min, luvInfo.v_max, frange(m_sp.luv.v));
            double r, g, b;
            luv2rgb(l, u, v, &r, &g, &b);
            return Color::fromRgbF((float) r, (float) g, (float) b, alphaF());
        }
        case Space::Lch: {
            double l = glm::mix(lchInfo.l_min, lchInfo.l_max, frange(m_sp.lch.l));
            double c = glm::mix(lchInfo.c_min, lchInfo.c_max, frange(m_sp.lch.c));
            double h = glm::mix(lchInfo.h_min, lchInfo.h_max, frange(m_sp.lch.h));
            double r, g, b;
            lch2rgb(l, c, h, &r, &g, &b);
            return Color::fromRgbF((float) r, (float) g, (float) b, alphaF());
        }
        case Space::Hsluv: {
            double h = us2deg(m_sp.hsluv.h);
            double s = frange(m_sp.hsluv.s) * 100.0;
            double l = frange(m_sp.hsluv.l) * 100.0;
            double r, g, b;
            hsluv2rgb(h, s, l, &r, &g, &b);
            return Color::fromRgbF((float) r, (float) g, (float) b, alphaF());
        }
    }
}

Color Color::toHsv() const {
    if (m_space == Space::Hsv)
        return *this;

    auto rgb = toRgb();
    float h, s, v;
    rgb2hsv(rgb.redF(), rgb.greenF(), rgb.blueF(), h, s, v);

    return Color::fromHsvF(h / 360.0f, s, v, alphaF());
}

Color Color::toLuv() const {
    if (m_space == Space::Luv)
        return *this;

    auto rgb = toRgb();
    double l, u, v;
    rgb2luv(rgb.redF(), rgb.greenF(), rgb.blueF(), &l, &u, &v);

    return Color::fromLuvF(
        posBetweenF(l, luvInfo.l_min, luvInfo.l_max),
        posBetweenF(u, luvInfo.u_min, luvInfo.u_max),
        posBetweenF(v, luvInfo.v_min, luvInfo.v_max),
        rgb.alphaF()
    );
}

Color Color::toLch() const {
    if (m_space == Space::Lch)
        return *this;

    auto rgb = toRgb();
    double l, c, h;
    rgb2lch(rgb.redF(), rgb.greenF(), rgb.blueF(), &l, &c, &h);

    return Color::fromLchF(
        posBetweenF(l, lchInfo.l_min, lchInfo.l_max),
        posBetweenF(c, lchInfo.c_min, lchInfo.c_max),
        posBetweenF(h, lchInfo.h_min, lchInfo.h_max),
        rgb.alphaF()
    );
}

Color Color::toHsluv() const {
    if (m_space == Space::Hsluv)
        return *this;

    auto rgb = toRgb();
    double h, s, l;
    rgb2hsluv(rgb.redF(), rgb.greenF(), rgb.blueF(), &h, &s, &l);

    return Color::fromHsluvF(
        static_cast<float>(h / 360.0),
        static_cast<float>(s / 100.0),
        static_cast<float>(l / 100.0),
        rgb.alphaF()
    );
}

Color Color::fromRgbF(float r, float g, float b, float a) {
    Color color;
    color.setRgbF(r, g, b, a);
    return color;
}

Color Color::fromHsv(int h, int s, int v, int a) {
    Color color;
    color.setHsv(h, s, v, a);
    return color;
}

Color Color::fromHsvF(float h, float s, float v, float a) {
    Color color;
    color.setHsvF(h, s, v, a);
    return color;
}

Color Color::fromLuv(int l, int u, int v, int a) {
    Color color;
    color.setLuv(l, u, v, a);
    return color;
}

Color Color::fromLuvF(float l, float u, float v, float a) {
    Color color;
    color.setLuvF(l, u, v, a);
    return color;
}

Color Color::fromLch(int l, int c, int h, int a) {
    Color color;
    color.setLch(l, c, h, a);
    return color;
}

Color Color::fromLchF(float l, float c, float h, float a) {
    Color color;
    color.setLchF(l, c, h, a);
    return color;
}

Color Color::fromHsluv(int h, int s, int l, int a) {
    Color color;
    color.setHsluv(h, s, l, a);
    return color;
}

Color Color::fromHsluvF(float h, float s, float l, float a) {
    Color color;
    color.setHsluvF(h, s, l, a);
    return color;
}

Color Color::parseColor(std::string color) {
    if (color.front() == '#') {
        color.erase(color.begin());
    }

    if (color.size() == 6) {
        color.insert(0, "ff");
    }

    auto colorValue = std::stoul(color, nullptr, 16);

    return Color(colorValue);
}
}
