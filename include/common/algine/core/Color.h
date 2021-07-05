#ifndef ALGINE_COLOR_H
#define ALGINE_COLOR_H

#include <algine/types.h>

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

    uint value() const;

    void setValue(uint color);
    void setRgb(int r, int g, int b, int a = 255);
    void setRgbF(float r, float g, float b, float a = 1.0f);

    static Color fromRgbF(float r, float g, float b, float a = 1.0f);

private:
    int m_red;
    int m_green;
    int m_blue;
    int m_alpha;
};
}

#endif //ALGINE_COLOR_H
