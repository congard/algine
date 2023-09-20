#ifndef ALGINE_INTERPOLATOR_H
#define ALGINE_INTERPOLATOR_H

#include <algine/platform.h>
#include <memory>

namespace algine::Widgets {
class AL_EXPORT Interpolator {
public:
    virtual float interpolate(float x) = 0;

    template<typename T, typename ...Args>
    inline static std::shared_ptr<T> create(Args&&... args) {
        static_assert(std::is_base_of_v<Interpolator, T>);
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

using InterpolatorPtr = std::shared_ptr<Interpolator>;
}

#endif //ALGINE_INTERPOLATOR_H
