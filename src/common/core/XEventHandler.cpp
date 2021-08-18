#include <algine/core/XEventHandler.h>

#ifdef __ANDROID__
    #include <algine/core/Screen.h>
#else
    #include <algine/core/window/Window.h>
#endif

namespace algine {
void XEventHandler::onPointerMove(PointerInfo info) {}
void XEventHandler::onPointerClick(PointerInfo info) {}
void XEventHandler::onPointerPress(PointerInfo info) {}
void XEventHandler::onPointerRelease(PointerInfo info) {}

void XEventHandler::onKeyboardKeyPress(KeyboardKey key) {}
void XEventHandler::onKeyboardKeyRelease(KeyboardKey key) {}
void XEventHandler::onKeyboardKeyRepeat(KeyboardKey key) {}

void XEventHandler::onSizeChanged(int width, int height) {}

#ifdef __ANDROID__
bool XEventHandler::PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return false;
    } else {
        return Screen::instance()->isPointerAvailable(std::get<int>(pointer));
    }
}

glm::vec2 XEventHandler::PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return {-1.0f, -1.0f};
    } else {
        return Screen::instance()->getPointerPos(std::get<int>(pointer));
    }
}

void XEventHandler::pointerDown(float x, float y, int pointerId) {
    onPointerPress({x, y, pointerId});
}

void XEventHandler::pointerMove(float x, float y, int pointerId) {
    onPointerMove({x, y, pointerId});
}

void XEventHandler::pointerUp(float x, float y, int pointerId) {
    onPointerRelease({x, y, pointerId});
}

void XEventHandler::pointerClick(float x, float y, int pointerId) {
    onPointerClick({x, y, pointerId});
}

void XEventHandler::surfaceResized(int width, int height) {
    onSizeChanged(width, height);
}
#else
bool XEventHandler::PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return false;
    } else {
        return m_window.isMouseKeyPressed(std::get<MouseKey>(pointer));
    }
}

glm::vec2 XEventHandler::PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return {-1.0f, -1.0f};
    } else {
        return m_pos;
    }
}

inline glm::vec2 toVec2(const glm::dvec2 &v) {
    return {
        static_cast<float>(v.x),
        static_cast<float>(v.y)
    };
}

void XEventHandler::mouseMove(double x, double y, Window &window) {
    onPointerMove({
        {static_cast<float>(x), static_cast<float>(y)},
        window
    });
}

void XEventHandler::mouseClick(MouseKey key, Window &window) {
    onPointerClick({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::mouseKeyPress(MouseKey key, Window &window) {
    onPointerPress({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::mouseKeyRelease(MouseKey key, Window &window) {
    onPointerRelease({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::keyboardKeyPress(KeyboardKey key, Window &window) {
    onKeyboardKeyPress(key);
}

void XEventHandler::keyboardKeyRelease(KeyboardKey key, Window &window) {
    onKeyboardKeyRelease(key);
}

void XEventHandler::keyboardKeyRepeat(KeyboardKey key, Window &window) {
    onKeyboardKeyRepeat(key);
}

void XEventHandler::windowSizeChange(int width, int height, Window &window) {
    onSizeChanged(width, height);
}
#endif
}
