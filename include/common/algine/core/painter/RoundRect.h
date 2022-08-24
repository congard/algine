#ifndef ALGINE_ROUNDRECT_H
#define ALGINE_ROUNDRECT_H

#include <algine/core/math/Rect.h>

namespace algine {
/**
 * A --------- B
 *  |         |
 *  |         |
 *  |         |
 * C --------- D
 * A - c1 (top left)
 * B - c2 (top right)
 * C - c3 (bottom left)
 * D - c4 (bottom right)
 */
class RoundRect {
public:
    class Corner {
    public:
        Corner() = default;
        Corner(float xRadius, float yRadius);
        explicit Corner(float radius);

        inline void setXRadius(float xRadius);
        inline void setYRadius(float yRadius);

        inline float getXRadius() const;
        inline float getYRadius() const;

    private:
        float m_xRadius;
        float m_yRadius;
    };

public:
    RoundRect(const RectF &rect, Corner c1, Corner c2, Corner c3, Corner c4);
    RoundRect(const RectF &rect, float r1, float r2, float r3, float r4);
    RoundRect(const RectF &rect, float radius);

    inline void setRect(const RectF &rect);

    inline void setTopLeftCorner(const Corner &corner);
    inline void setTopRightCorner(const Corner &corner);
    inline void setBottomLeftCorner(const Corner &corner);
    inline void setBottomRightCorner(const Corner &corner);

    inline const Corner& getTopLeftCorner() const;
    inline const Corner& getTopRightCorner() const;
    inline const Corner& getBottomLeftCorner() const;
    inline const Corner& getBottomRightCorner() const;

    inline const RectF& getRect() const;

private:
    RectF m_rect;
    Corner m_c1;
    Corner m_c2;
    Corner m_c3;
    Corner m_c4;
};

inline RoundRect::Corner::Corner(float xRadius, float yRadius)
    : m_xRadius(xRadius), m_yRadius(yRadius) {}

inline RoundRect::Corner::Corner(float radius)
    : Corner(radius, radius) {}

inline void RoundRect::Corner::setXRadius(float xRadius) {
    m_xRadius = xRadius;
}

inline void RoundRect::Corner::setYRadius(float yRadius) {
    m_yRadius = yRadius;
}

inline float RoundRect::Corner::getXRadius() const {
    return m_xRadius;
}

inline float RoundRect::Corner::getYRadius() const {
    return m_yRadius;
}

inline RoundRect::RoundRect(const RectF &rect, Corner c1, Corner c2, Corner c3, Corner c4)
    : m_rect(rect), m_c1(c1), m_c2(c2), m_c3(c3), m_c4(c4) {}

inline RoundRect::RoundRect(const RectF &rect, float r1, float r2, float r3, float r4)
    : RoundRect(rect, Corner(r1), Corner(r2), Corner(r3), Corner(r4)) {}

inline RoundRect::RoundRect(const RectF &rect, float radius)
    : RoundRect(rect, radius, radius, radius, radius) {}

inline void RoundRect::setRect(const RectF &rect) {
    m_rect = rect;
}

inline void RoundRect::setTopLeftCorner(const Corner &corner) {
    m_c1 = corner;
}

inline void RoundRect::setTopRightCorner(const Corner &corner) {
    m_c2 = corner;
}

inline void RoundRect::setBottomLeftCorner(const Corner &corner) {
    m_c3 = corner;
}

inline void RoundRect::setBottomRightCorner(const Corner &corner) {
    m_c4 = corner;
}

inline const RoundRect::Corner &RoundRect::getTopLeftCorner() const {
    return m_c1;
}

inline const RoundRect::Corner &RoundRect::getTopRightCorner() const {
    return m_c2;
}

inline const RoundRect::Corner &RoundRect::getBottomLeftCorner() const {
    return m_c3;
}

inline const RoundRect::Corner &RoundRect::getBottomRightCorner() const {
    return m_c4;
}

inline const RectF &RoundRect::getRect() const {
    return m_rect;
}
}

#endif //ALGINE_ROUNDRECT_H
