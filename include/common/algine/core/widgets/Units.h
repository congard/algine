#ifndef ALGINE_UNITS_H
#define ALGINE_UNITS_H

#include <algine/core/widgets/Dimen.h>
#include <algine/core/Engine.h>

namespace algine {
class Units {
public:
    template<typename T>
    static inline T dp(T value) {
        return static_cast<T>((float) value * ((float) Engine::getDPI() / 96.0f));
    }

    static inline Dimen parse_dimen(const char *str) {
        bool error;
        auto value = try_parse_dimen(str, &error);

        if (error) {
            throw std::runtime_error("Units::parse_dimen: parsing failed");
        }

        return value;
    }

    static Dimen try_parse_dimen(const char *str, bool *error);

    template<typename T = float>
    static inline T parse(const char *str) {
        return static_cast<T>(parse_dimen(str).pixels());
    }

    template<typename T = float>
    static inline T try_parse(const char *str, bool *error) {
        return static_cast<T>(try_parse_dimen(str, error).pixels());
    }
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
