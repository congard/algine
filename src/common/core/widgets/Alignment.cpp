#include <algine/core/widgets/Alignment.h>

#include <ranges>

namespace algine {
uint Alignment::parse(const char *str) {
    using std::operator""sv;

    uint alignment = Alignment::Center;

    for (const auto r : std::views::split(std::string_view {str}, "|"sv)) {
        if (std::string_view value {r}; value == "center") {
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
