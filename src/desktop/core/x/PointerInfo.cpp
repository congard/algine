#include <algine/core/x/PointerInfo.h>
#include <algine/core/window/Window.h>

namespace algine {
bool PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return false;
    } else {
        return m_window.isMouseKeyPressed(std::get<MouseKey>(pointer));
    }
}

glm::vec2 PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return {-1.0f, -1.0f};
    } else {
        return m_pos;
    }
}
}
