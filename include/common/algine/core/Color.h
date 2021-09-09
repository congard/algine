#ifndef ALGINE_COLOR_H
#define ALGINE_COLOR_H

#include <algine/types.h>

#include <string>

namespace algine {
class Color {
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

    void setValue(uint color);
    void setRgb(int r, int g, int b, int a = 255);
    void setRgbF(float r, float g, float b, float a = 1.0f);

    std::string toString() const;

    static Color fromRgbF(float r, float g, float b, float a = 1.0f);
    static Color parseColor(std::string color);

private:
    int m_red;
    int m_green;
    int m_blue;
    int m_alpha;
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
