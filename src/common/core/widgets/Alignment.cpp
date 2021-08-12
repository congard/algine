#include <algine/core/widgets/Alignment.h>

#include <tulz/StringUtils.h>

namespace algine {
uint Alignment::parse(const char *str) {
    auto alignmentValues = tulz::StringUtils::split(str, "|");
    uint alignment = Alignment::Center;

    for (std::string &value : alignmentValues) {
        if (value == "center") {
            alignment |= Alignment::Center;
        } else if (value == "left") {
            alignment |= Alignment::Left;
        } else if (value == "right") {
            alignment |= Alignment::Right;
        } else if (value == "top") {
            alignment |= Alignment::Top;
        } else if (value == "bottom") {
            alignment |= Alignment::Bottom;
        }
    }

    return alignment;
}
}
