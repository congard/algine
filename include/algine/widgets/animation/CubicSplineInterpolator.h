#ifndef ALGINE_CUBICSPLINEINTERPOLATOR_H
#define ALGINE_CUBICSPLINEINTERPOLATOR_H

#include <algine/math/CubicSpline.h>
#include <algine/widgets/animation/Interpolator.h>

namespace algine::Widgets {
class AL_EXPORT CubicSplineInterpolator: public Interpolator {
public:
    CubicSplineInterpolator();
    explicit CubicSplineInterpolator(CubicSpline spline);

    void setSpline(CubicSpline spline);
    const CubicSpline& getSpline() const;

    float interpolate(float x) override;

private:
    CubicSpline m_spline;
};
}

#endif //ALGINE_CUBICSPLINEINTERPOLATOR_H
