#ifndef ALGINE_BEZIERCURVE_H
#define ALGINE_BEZIERCURVE_H

#include <vector>

namespace algine {
template<typename T>
class BezierCurve {
public:
    BezierCurve() = default;
    explicit BezierCurve(const std::vector<T> &points): m_points(points) {}

    void setPoints(const std::vector<T> &points) { m_points = points; }
    const std::vector<T>& getPoints() const { return m_points; }

    T getPoint(float t) const { return getPoint(m_points, t); }

    static T getPoint(std::vector<T> points, float t) {
        int i = points.size() - 1;

        while (i > 0) {
            for (int k = 0; k < i; k++) {
                points[k] = points[k] + t * (points[k + 1] - points[k]);
            }

            i--;
        }

        return points[0];
    }

private:
    std::vector<T> m_points;
};
} // algine

#endif //ALGINE_BEZIERCURVE_H
