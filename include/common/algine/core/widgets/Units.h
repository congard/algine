#ifndef ALGINE_UNITS_H
#define ALGINE_UNITS_H

#include <algine/core/Engine.h>

namespace algine {
class Units {
public:
    template<typename T>
    static inline T dp(T value) {
        return static_cast<T>((float) value * ((float) Engine::getDPI() / 96.0f));
    }

    template<typename T = float>
    static inline T parse(const char *str) {
        bool error;
        auto value = static_cast<T>(parseImpl(str, &error));

        if (error) {
            throw std::runtime_error("Units::parse: parsing failed");
        }

        return value;
    }

    template<typename T = float>
    static inline T try_parse(const char *str, bool *error) {
        return static_cast<T>(parseImpl(str, error));
    }

private:
    static float parseImpl(const char *str, bool *error);
};

inline auto operator""_px(unsigned long long int value) {
    return value;
}

inline auto operator""_px(long double value) {
    return value;
}

inline auto operator""_dp(unsigned long long int value) {
    return Units::dp(value);
}

inline auto operator""_dp(long double value) {
    return Units::dp(value);
}
}

#endif //ALGINE_UNITS_H
