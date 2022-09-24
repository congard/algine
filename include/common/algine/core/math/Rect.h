#ifndef ALGINE_RECT_H
#define ALGINE_RECT_H

#include <algine/core/math/Point.h>

#include <algorithm>

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

    /**
     * @param rectangle
     * @returns the bounding rectangle of this rectangle and the given rectangle.
     * @see united(), unite(), operator|=()
     */
    Rect<T> operator|(const Rect<T> &rectangle) const {
        return united(rectangle);
    }

    /**
     * Unites this rectangle with the given rectangle.
     * @param rectangle
     * @returns this
     * @see united(), unite(), operator|()
     */
    Rect<T> operator|=(const Rect<T> &rectangle) {
        unite(rectangle);
        return *this;
    }

    /**
     * @param rectangle
     * @returns the intersection of this rectangle and the given rectangle.
     * Returns an empty rectangle if there is no intersection.
     * @see intersected(), intersect(), operator&=()
     */
    Rect<T> operator&(const Rect<T> &rectangle) const {
        return intersected(rectangle);
    }

    /**
     * Intersects this rectangle with the given rectangle.
     * @param rectangle
     * @returns this
     * @see intersected(), intersect(), operator&()
     */
    Rect<T>& operator&=(const Rect<T> &rectangle) {
        intersect(rectangle);
        return *this;
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

    void translate(T dx, T dy) {
        m_x += dx;
        m_y += dy;
    }

    void translate(const Point<T> &offset) {
        m_x += offset.getX();
        m_y += offset.getY();
    }

    void unite(const Rect<T> &r) {
        *this = united(r);
    }

    /**
     * @param r rectangle to unite
     * @returns the bounding rectangle of this rectangle and the given rectangle r
     */
    Rect<T> united(const Rect<T> &r) const {
        Point<T> p1 {
            std::min(left(), r.left()),
            std::min(top(), r.top())
        };

        Point<T> p2 {
            std::max(right(), r.right()),
            std::max(bottom(), r.bottom())
        };

        return {p1, p2};
    }

    void intersect(const Rect<T> &r) {
        *this = intersected(r);
    }

    /**
     * @param r rectangle to intersect
     * @returns the intersection of this rectangle and the given rectangle
     */
    Rect<T> intersected(const Rect<T> &r) const {
        if (left() > r.right() || top() > r.bottom() || right() < r.left() || bottom() < r.top())
            return {0, 0, 0, 0};

        Point<T> p1 {
            std::max(left(), r.left()),
            std::max(top(), r.top())
        };

        Point<T> p2 {
            std::min(right(), r.right()),
            std::min(bottom(), r.bottom())
        };

        return {p1, p2};
    }

    /**
     * @param r rectangle to intersect
     * @returns true if this rectangle intersects with the given rectangle
     * (i.e. there is a non-empty area of overlap between them), otherwise returns false.
     * @note The intersection rectangle can be retrieved using the intersected() function.
     * @see intersected()
     */
    bool intersects(const Rect<T> &r) const {
        return intersected(r).isValid();
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

    T top() const {
        return getY();
    }

    T left() const {
        return getX();
    }

    T bottom() const {
        return getY() + getHeight();
    }

    T right() const {
        return getX() + getWidth();
    }

    Point<T> topLeft() const {
        return {left(), top()};
    }

    Point<T> topRight() const {
        return {right(), top()};
    }

    Point<T> bottomLeft() const {
        return {left(), bottom()};
    }

    Point<T> bottomRight() const {
        return {right(), bottom()};
    }

    bool contains(const Point<T> &p) const {
        return p.getX() >= left() && p.getY() >= top() && p.getX() <= right() && p.getY() <= bottom();
    }

    bool contains(const Rect<T> &r) const {
        return contains(r.topLeft()) && contains(r.bottomRight());
    }

    bool isValid() const {
        return m_width > T(0) && m_height > T(0);
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
