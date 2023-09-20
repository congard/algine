#ifndef ALGINE_COLOR_H
#define ALGINE_COLOR_H

#include <algine/platform.h>
#include <algine/types.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

namespace algine {
class AL_EXPORT Color {
public:
    enum class Space {
        Invalid, Rgb, Hsv, Luv, Lch, Hsluv
    };

public:
    Color();
    explicit Color(uint color);
    Color(int r, int g, int b, int a = 255);

    bool operator==(const Color &rhs) const;
    bool operator!=(const Color &rhs) const;

    int red() const;
    int green() const;
    int blue() const;
    int alpha() const;

    float redF() const;
    float greenF() const;
    float blueF() const;
    float alphaF() const;

    template<typename T> auto red() const;
    template<typename T> auto green() const;
    template<typename T> auto blue() const;
    template<typename T> auto alpha() const;

    uint value() const;

    /**
     * @return <code>false</code> if either <code>getSpace() == Space::Invalid</code>
     * or converting to RGB gives values out-of-range, otherwise <code>true</code>
     */
    bool isValid() const;

    void setValue(uint color);
    void setRgb(int r, int g, int b, int a = 255);
    void setRgbF(float r, float g, float b, float a = 1.0f);

    void setRed(int red);
    void setGreen(int green);
    void setBlue(int blue);
    void setAlpha(int alpha);

    void setRedF(float red);
    void setGreenF(float green);
    void setBlueF(float blue);
    void setAlphaF(float alpha);

    /**
     * Rotates HSV hue component
     * @param degrees in range [0, 360]
     * @return new color
     */
    Color hsvHueRotate(int degrees) const;

    /**
     * Rotates HSV hue component
     * @param val in range [0, 1]
     * @return new color
     */
    Color hsvHueRotateF(float val) const;

    /**
     * Rotates LCH hue component
     * @param degrees in range [0, 360]
     * @return new color
     */
    Color lchHueRotate(int degrees) const;

    /**
     * Rotates LCH hue component
     * @param val in range [0, 1]
     * @return new color
     */
    Color lchHueRotateF(float val) const;

    /**
     * Rotates HSLuv hue component
     * @param degrees in range [0, 360]
     * @return new color
     */
    Color hsluvHueRotate(int degrees) const;

    /**
     * Rotates HSLuv hue component
     * @param val in range [0, 1]
     * @return new color
     */
    Color hsluvHueRotateF(float val) const;

    glm::ivec3 getRgb() const;
    glm::vec3 getRgbF() const;

    glm::ivec3 getHsv() const;
    glm::vec3 getHsvF() const;

    glm::ivec3 getLuv() const;
    glm::vec3 getLuvF() const;

    glm::ivec3 getLch() const;
    glm::vec3 getLchF() const;

    glm::ivec3 getHsluv() const;
    glm::vec3 getHsluvF() const;

    /**
     * @param h hue, range [0, 360]
     * @param s saturation, range [0, 255]
     * @param v value, range [0, 255]
     * @param a alpha, range [0, 255]
     * @see toHsv, setHsvF, getHsv, getHsvF
     */
    void setHsv(int h, int s, int v, int a = 255);

    /**
     * @param h hue, range [0.0, 1.0]
     * @param s saturation, range [0.0, 1.0]
     * @param v value, range [0.0, 1.0]
     * @param a alpha, range [0.0, 1.0]
     * @see toHsv, setHsv, getHsv, getHsvF
     */
    void setHsvF(float h, float s, float v, float a = 1.0f);

    /**
     * @param l range [0, 255]
     * @param u range [0, 255]
     * @param v range [0, 255]
     * @param a range [0, 255]
     * @see toLuv, setLuvF, getLuv, getLuvF
     */
    void setLuv(int l, int u, int v, int a = 255);

    /**
     * @param l range [0.0, 1.0]
     * @param u range [0.0, 1.0]
     * @param v range [0.0, 1.0]
     * @param a range [0.0, 1.0]
     * @see toLuv, setLuv, getLuv, getLuvF
     */
    void setLuvF(float l, float u, float v, float a = 1.0f);

    /**
     * @param l lightness, range [0, 255]
     * @param c chroma, range [0, 255]
     * @param h hue, range [0, 360]
     * @param a alpha, range [0, 255]
     * @see toLch, setLchF, getLch, getLchF
     */
    void setLch(int l, int c, int h, int a = 255);

    /**
     * @param l lightness, range [0.0, 1.0]
     * @param c chroma, range [0.0, 1.0]
     * @param h hue, range [0.0, 1.0]
     * @param a alpha, range [0.0, 1.0]
     * @see toLch, setLch, getLch, getLchF
     */
    void setLchF(float l, float c, float h, float a = 1.0f);

    /**
     * Human-friendly alternative to HSL. More info: https://www.hsluv.org/
     * @param h hue, range [0, 360]
     * @param s saturation, range [0, 255]
     * @param l lightness, range [0, 255]
     * @param a alpha, range [0, 255]
     * @see toHsluv, setHsluvF, getHsluv, getHsluvF
     */
    void setHsluv(int h, int s, int l, int a = 255);

    /**
     * Human-friendly alternative to HSL. More info: https://www.hsluv.org/
     * @param h hue, range [0.0, 1.0]
     * @param s saturation, range [0.0, 1.0]
     * @param l lightness, range [0.0, 1.0]
     * @param a alpha, range [0.0, 1.0]
     * @see toHsluv, setHsluv, getHsluv, getHsluvF
     */
    void setHsluvF(float h, float s, float l, float a = 1.0f);

    std::string toString() const;

    Space getSpace() const;

    /**
     * @return <code>Color</code> in RGB space; if converting to RGB results in
     * out-of-range values, they will be clamped to the correct range
     * @see isValid
     */
    Color toRgb() const;

    Color toHsv() const;
    Color toLuv() const;
    Color toLch() const;
    Color toHsluv() const;

    static Color fromRgb(int r, int g, int b, int a = 255);
    static Color fromRgbF(float r, float g, float b, float a = 1.0f);
    static Color fromRgbF(const glm::vec4 &rgba);

    static Color fromHsv(int h, int s, int v, int a = 255);
    static Color fromHsvF(float h, float s, float v, float a = 1.0f);
    static Color fromHsvF(const glm::vec4 &hsva);

    static Color fromLuv(int l, int u, int v, int a = 255);
    static Color fromLuvF(float l, float u, float v, float a = 1.0f);
    static Color fromLuvF(const glm::vec4 &luva);

    static Color fromLch(int l, int c, int h, int a = 255);
    static Color fromLchF(float l, float c, float h, float a = 1.0f);
    static Color fromLchF(const glm::vec4 &lcha);

    static Color fromHsluv(int h, int s, int l, int a = 255);
    static Color fromHsluvF(float h, float s, float l, float a = 1.0f);
    static Color fromHsluvF(const glm::vec4 &hsla);

    static Color parseColor(std::string color);

    /**
     * Mixes x and y colors in RGB space using position a
     * @param x color x
     * @param y color y
     * @param a position in range [0, 1]
     * @return interpolated color
     */
    static Color rgbMix(const Color &x, const Color &y, float a);

    /**
     * Mixes x and y colors in HSV space using position a
     * @param x color x
     * @param y color y
     * @param a position in range [0, 1]
     * @return interpolated color
     */
    static Color hsvMix(const Color &x, const Color &y, float a);

    /**
     * Mixes x and y colors in LUV space using position a
     * @param x color x
     * @param y color y
     * @param a position in range [0, 1]
     * @return interpolated color
     */
    static Color luvMix(const Color &x, const Color &y, float a);

    /**
     * Mixes x and y colors in LCH space using position a
     * @param x color x
     * @param y color y
     * @param a position in range [0, 1]
     * @return interpolated color
     */
    static Color lchMix(const Color &x, const Color &y, float a);

    /**
     * Mixes x and y colors in HSLuv space using position a
     * @param x color x
     * @param y color y
     * @param a position in range [0, 1]
     * @return interpolated color
     */
    static Color hsluvMix(const Color &x, const Color &y, float a);

private:
    union sp {
        inline explicit sp(uint16 p1, uint16 p2, uint16 p3, uint16 p4)
            : rgb({p1, p2, p3, p4}) {}

        struct { uint16 r, g, b, a; } rgb;
        struct { uint16 h, s, v, a; } hsv;
        struct { uint16 l, u, v, a; } luv;
        struct { uint16 l, c, h, a; } lch;
        struct { uint16 h, s, l, a; } hsluv;
    } m_sp;

    Space m_space {Space::Invalid};
};

template<typename T>
auto Color::red() const {
    if constexpr(std::is_same_v<T, int>) {
        return red();
    }

    if constexpr(std::is_same_v<T, float>) {
        return redF();
    }
}

template<typename T>
auto Color::green() const {
    if constexpr(std::is_same_v<T, int>) {
        return green();
    }

    if constexpr(std::is_same_v<T, float>) {
        return greenF();
    }
}

template<typename T>
auto Color::blue() const {
    if constexpr(std::is_same_v<T, int>) {
        return blue();
    }

    if constexpr(std::is_same_v<T, float>) {
        return blueF();
    }
}

template<typename T>
auto Color::alpha() const {
    if constexpr(std::is_same_v<T, int>) {
        return alpha();
    }

    if constexpr(std::is_same_v<T, float>) {
        return alphaF();
    }
}
}

#endif //ALGINE_COLOR_H
