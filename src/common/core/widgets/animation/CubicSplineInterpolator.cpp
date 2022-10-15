#include <algine/core/widgets/animation/CubicSplineInterpolator.h>

#include <utility>

namespace algine::Widgets {
CubicSplineInterpolator::CubicSplineInterpolator() {
    m_spline = CubicSpline({
        {0.0f, 0.0f},
        {0.5f, 0.25f},
        {1.0f, 1.0f},
        {1.5f, 0.25f},
        {2.0f, 0.0f}
    });
}

CubicSplineInterpolator::CubicSplineInterpolator(CubicSpline spline)
    : m_spline(std::move(spline)) {}

void CubicSplineInterpolator::setSpline(CubicSpline spline) {
    m_spline = std::move(spline);
}

const CubicSpline& CubicSplineInterpolator::getSpline() const {
    return m_spline;
}

float CubicSplineInterpolator::interpolate(float x) {
    return m_spline.getY(x);
}
}
