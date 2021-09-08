#include <algine/core/widgets/Dimen.h>
#include <algine/core/widgets/Units.h>

namespace algine {
float Dimen::pixels() const {
    if (m_unit == Unit::dp) {
        return Units::dp(m_value);
    } else {
        return m_value;
    }
}

std::string Dimen::toString() const {
    return std::to_string(m_value) + [this]() {
        switch (m_unit) {
            case Unit::dp: return "dp";
            default: return "px";
        }
    }();
}
}
