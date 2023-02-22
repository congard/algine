#include <algine/core/unified/UnifiedEventHandler.h>
#include <algine/core/window/Window.h>

namespace algine {
inline glm::vec2 toVec2(const glm::dvec2 &v) {
    return {
        static_cast<float>(v.x),
        static_cast<float>(v.y)
    };
}

void UnifiedEventHandler::mouseMove(double x, double y, Window &window) {
    onPointerMove({
        {static_cast<float>(x), static_cast<float>(y)},
        window
    });
}

void UnifiedEventHandler::mouseClick(MouseKey key, Window &window) {
    onPointerClick({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::mouseKeyPress(MouseKey key, Window &window) {
    onPointerPress({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::mouseKeyRelease(MouseKey key, Window &window) {
    onPointerRelease({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::keyboardKeyPress(KeyboardKey key, Window &window) {
    onKeyboardKeyPress(key);
}

void UnifiedEventHandler::keyboardKeyRelease(KeyboardKey key, Window &window) {
    onKeyboardKeyRelease(key);
}

void UnifiedEventHandler::keyboardKeyRepeat(KeyboardKey key, Window &window) {
    onKeyboardKeyRepeat(key);
}

void UnifiedEventHandler::windowSizeChange(int width, int height, Window &window) {
    onSizeChanged(width, height);
}

void UnifiedEventHandler::windowIconify(Window &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void UnifiedEventHandler::windowRestore(Window &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void UnifiedEventHandler::windowFocusLost(Window &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void UnifiedEventHandler::windowFocus(Window &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
}
