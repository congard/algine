#include <algine/core/x/PointerInfo.h>
#include <algine/core/Screen.h>

namespace algine {
bool PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return false;
    } else {
        return Screen::instance()->isPointerAvailable(std::get<Finger>(pointer));
    }
}

glm::vec2 PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return {-1.0f, -1.0f};
    } else {
        return Screen::instance()->getPointerPos(std::get<Finger>(pointer));
    }
}
}
