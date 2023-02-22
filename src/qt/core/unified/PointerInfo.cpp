#include <algine/core/unified/PointerInfo.h>
#include <algine/core/window/QtWindow.h>

namespace algine {
bool PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<Finger>(&pointer) != nullptr) { // not available on Qt as for now
        return false;
    } else {
        return m_window.isMouseKeyPressed(std::get<MouseKey>(pointer));
    }
}

glm::vec2 PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<Finger>(&pointer) != nullptr) { // not available on Qt as for now
        return {-1.0f, -1.0f};
    } else {
        return m_pos;
    }
}
}
