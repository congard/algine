#include <algine/core/widgets/Units.h>

#include <tulz/Array.h>

#include <cstring>

namespace algine {
float Units::parseImpl(const char *str, bool *error) {
    enum class Unit {
        px, dp
    };

    Unit unit = Unit::px;

    constexpr auto dp_suffix = "dp";
    constexpr auto px_suffix = "px";

    auto str_len = strlen(str);

    auto isUnit = [&](const char *suffix, Unit _unit) {
        auto suffix_len = strlen(suffix);

        if (strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0) {
            str_len -= suffix_len;
            unit = _unit;
            return true;
        }

        return false;
    };

    isUnit(dp_suffix, Unit::dp) || isUnit(px_suffix, Unit::px);

    tulz::Array<char> data(str_len + 1);
    memcpy(data.array(), str, str_len);
    data[str_len] = '\0';

    char *end;

    if (float float_value = strtof(data.array(), &end); *end == '\0') {
        if (error != nullptr)
            *error = false;

        if (unit == Unit::dp) {
            return Units::dp(float_value);
        } else {
            return float_value;
        }
    } else {
        if (error != nullptr)
            *error = true;

        return 0.0f;
    }
}
}
