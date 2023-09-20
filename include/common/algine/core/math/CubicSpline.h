#ifndef ALGINE_CUBICSPLINE_H
#define ALGINE_CUBICSPLINE_H

#include <algine/platform.h>
#include <vector>
#include <glm/vec2.hpp>

namespace algine {
class AL_EXPORT CubicSpline {
public:
    CubicSpline();
    explicit CubicSpline(const std::vector<glm::vec2> &points);

    void setPoints(const std::vector<glm::vec2> &points);
    const std::vector<glm::vec2>& getPoints() const;

    float getY(float x) const;
    glm::vec2 getPoint(float t) const;

private:
    struct SplineSet {
        float b;
        float c;
        float d;
    };

    std::vector<glm::vec2> m_points;
    std::vector<SplineSet> m_spline;
};
} // algine

#endif //ALGINE_CUBICSPLINE_H
