#ifndef ALGINE_RECT_H
#define ALGINE_RECT_H

#include "Point.h"

namespace algine {
template<typename T>
class Rect {
public:
    Rect() = default;

    Rect(T x, T y, T width, T height)
        : m_x(x), m_y(y),
          m_width(width), m_height(height) {}

    Rect(const Point<T> &p1, const Point<T> &p2)
        : m_x(p1.getX()), m_y(p1.getY()),
          m_width(p2.getX() - p1.getX()),
          m_height(p2.getY() - p1.getY()) {}

    bool operator==(const Rect &rhs) const {
        return (m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_width == rhs.m_width) && (m_height == rhs.m_height);
    }

    bool operator!=(const Rect &rhs) const {
        return !(*this == rhs);
    }

    void setPos(T x, T y) {
        m_x = x;
        m_y = y;
    }

    void setCoords(T x1, T y1, T x2, T y2) {
        m_x = x1;
        m_y = y1;
        m_width = x2 - x1;
        m_height = y2 - y1;
    }

    void setX(T x) {
        m_x = x;
    }

    void setY(T y) {
        m_y = y;
    }

    void setWidth(T width) {
        m_width = width;
    }

    void setHeight(T height) {
        m_height = height;
    }

    T getX() const {
        return m_x;
    }

    T getY() const {
        return m_y;
    }

    T getWidth() const {
        return m_width;
    }

    T getHeight() const {
        return m_height;
    }

private:
    T m_x;
    T m_y;
    T m_width;
    T m_height;
};

using RectI = Rect<int>;
using RectF = Rect<float>;
}

#endif //ALGINE_RECT_H
