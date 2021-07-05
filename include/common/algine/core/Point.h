#ifndef ALGINE_POINT_H
#define ALGINE_POINT_H

namespace algine {
template<typename T>
class Point {
public:
    Point() = default;

    Point(T x, T y)
        : m_x(x), m_y(y) {}

    bool operator==(const Point &rhs) const {
        return (m_x == rhs.m_x) && (m_y == rhs.m_y);
    }

    bool operator!=(const Point &rhs) const {
        return !(*this == rhs);
    }

    Point operator+(const Point &rhs) const {
        return {m_x + rhs.m_x, m_y + rhs.m_y};
    }

    Point operator-(const Point &rhs) const {
        return {m_x - rhs.m_x, m_y - rhs.m_y};
    }

    Point operator*(T value) const {
        return {m_x * value, m_y * value};
    }

    Point operator/(T value) const {
        return {m_x / value, m_y / value};
    }

    Point& operator+=(const Point &rhs) {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }

    Point& operator-=(const Point &rhs) {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        return *this;
    }

    Point& operator*=(T value) {
        m_x *= value;
        m_y *= value;
        return *this;
    }

    Point& operator/=(T value) {
        m_x /= value;
        m_y /= value;
        return *this;
    }

    void setPos(T x, T y) {
        m_x = x;
        m_y = y;
    }

    void setX(T x) {
        m_x = x;
    }

    void setY(T y) {
        m_y = y;
    }

    T& x() {
        return m_x;
    }

    T& y() {
        return m_y;
    }

    T getX() const {
        return m_x;
    }

    T getY() const {
        return m_y;
    }

private:
    T m_x;
    T m_y;
};
}

#endif //ALGINE_POINT_H
