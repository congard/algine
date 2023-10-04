#include <algine/core/widgets/Dimen.h>
#include <algine/core/Window.h>

#include <sstream>

namespace algine {
namespace DimenStrings {
constexpr static auto dp = "dp";
constexpr static auto px = "px";

namespace detail {
constexpr static size_t constexpr_strlen(const char *str) {
    if (*str == '\0')
        return 0;
    return 1 + constexpr_strlen(str + 1);
}

static_assert(constexpr_strlen(px) == constexpr_strlen(dp));
}

constexpr static auto suffix_len = detail::constexpr_strlen(dp);
}

Dimen::Dimen(std::string_view str) {
    if (!parse(str)) {
        throw std::invalid_argument("Invalid argument: the provided data cannot be parsed as dimen");
    }
}

float Dimen::pixels(Window *window) const {
    if (m_unit == Unit::dp) {
        return m_value * (static_cast<float>(window->getDPI()) / 96.0f);
    } else {
        return m_value;
    }
}

std::string Dimen::toString() const {
    std::ostringstream valueStrRep;
    valueStrRep << std::defaultfloat << m_value;

    return valueStrRep.str() + [this]() {
        switch (m_unit) {
            case Unit::dp: return DimenStrings::dp;
            default: return DimenStrings::px;
        }
    }();
}

bool Dimen::parse(std::string_view str) {
    // detect unit
    if (str.ends_with(DimenStrings::dp)) {
        setUnit(Unit::dp);
    } else if (str.ends_with(DimenStrings::px)) {
        setUnit(Unit::px);
    } else {
        return false;
    }

    // read value
    char *end;
    float value = std::strtof(str.data(), &end);

    if (end - str.data() == str.length() - DimenStrings::suffix_len) {
        setValue(value);
        return true;
    }

    return false;
}
}
