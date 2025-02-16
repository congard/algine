#ifndef ALGINE_SIZE_H
#define ALGINE_SIZE_H

namespace algine {
/**
 * The Size class defines the size of a
 * two-dimensional object using T point precision
 * @tparam T
 */
template<typename T>
class Size {
public:
    Size(): m_width(-1), m_height(-1) {}

    Size(T width, T height):
        m_width(width), m_height(height) {}

    bool operator==(const Size &rhs) const {
        return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    bool operator!=(const Size &rhs) const {
        return !(*this == rhs);
    }

    void setWidth(T width) {
        m_width = width;
    }

    void setHeight(T height) {
        m_height = height;
    }

    T& width() {
        return m_width;
    }

    T& height() {
        return m_height;
    }

    T getWidth() const {
        return m_width;
    }

    T getHeight() const {
        return m_height;
    }

    bool isValid() const {
        return m_width >= T(0) && m_height >= T(0);
    }

private:
    T m_width;
    T m_height;
};

using SizeI = Size<int>;
using SizeF = Size<float>;
} // algine

#endif //ALGINE_SIZE_H
