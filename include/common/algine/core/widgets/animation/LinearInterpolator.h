#ifndef ALGINE_LINEARINTERPOLATOR_H
#define ALGINE_LINEARINTERPOLATOR_H

#include <algine/core/widgets/animation/Interpolator.h>

namespace algine::Widgets {
class LinearInterpolator: public Interpolator {
public:
    float interpolate(float x) override;
};
}

#endif //ALGINE_LINEARINTERPOLATOR_H
