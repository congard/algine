#ifndef ALGINE_COLOR_H
#define ALGINE_COLOR_H

namespace algine {
class Color {
public:
    Color();
    explicit Color(int color);
    Color(int r, int g, int b, int a = 255);

    int red() const;
    int green() const;
    int blue() const;
    int alpha() const;

    float redF() const;
    float greenF() const;
    float blueF() const;
    float alphaF() const;

    int value() const;

    void setValue(int color);
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
