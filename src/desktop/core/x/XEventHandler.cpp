#include <algine/core/x/XEventHandler.h>
#include <algine/core/window/Window.h>

namespace algine {
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

void XEventHandler::windowIconify(Window &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void XEventHandler::windowRestore(Window &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void XEventHandler::windowFocusLost(Window &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void XEventHandler::windowFocus(Window &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
}
