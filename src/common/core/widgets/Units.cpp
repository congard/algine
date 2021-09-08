#include <algine/core/widgets/Units.h>
#include <algine/core/widgets/Unit.h>

#include <tulz/Array.h>

#include <cstring>

namespace algine {
Dimen Units::try_parse_dimen(const char *str, bool *error) {
    Dimen dimen;

    constexpr auto dp_suffix = "dp";
    constexpr auto px_suffix = "px";

    auto str_len = strlen(str);

    auto isUnit = [&](const char *suffix, Unit unit) {
        auto suffix_len = strlen(suffix);

        if (strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0) {
            str_len -= suffix_len;
            dimen.setUnit(unit);
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

        dimen.setValue(float_value);
    } else if (error != nullptr) {
        *error = true;
    }

    return dimen;
}
}
