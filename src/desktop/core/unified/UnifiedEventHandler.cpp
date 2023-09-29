#include <algine/core/unified/UnifiedEventHandler.h>
#include <algine/core/window/GLFWWindow.h>

namespace algine {
inline static glm::vec2 toVec2(const glm::dvec2 &v) {
    return {
        static_cast<float>(v.x),
        static_cast<float>(v.y)
    };
}

void UnifiedEventHandler::mouseMove(double x, double y, GLFWWindow &window) {
    onPointerMove({
        {static_cast<float>(x), static_cast<float>(y)},
        window
    });
}

void UnifiedEventHandler::mouseClick(MouseKey key, GLFWWindow &window) {
    onPointerClick({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::mouseKeyPress(MouseKey key, GLFWWindow &window) {
    onPointerPress({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::mouseKeyRelease(MouseKey key, GLFWWindow &window) {
    onPointerRelease({toVec2(window.getCursorPos()), key, window});
}

void UnifiedEventHandler::keyboardKeyPress(KeyboardKey key, GLFWWindow&) {
    onKeyboardKeyPress(key);
}

void UnifiedEventHandler::keyboardKeyRelease(KeyboardKey key, GLFWWindow&) {
    onKeyboardKeyRelease(key);
}

void UnifiedEventHandler::keyboardKeyRepeat(KeyboardKey key, GLFWWindow&) {
    onKeyboardKeyRepeat(key);
}

void UnifiedEventHandler::windowSizeChange(int width, int height, GLFWWindow&) {
    onSizeChanged(width, height);
}

void UnifiedEventHandler::windowIconify(GLFWWindow &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void UnifiedEventHandler::windowRestore(GLFWWindow &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void UnifiedEventHandler::windowFocusLost(GLFWWindow &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void UnifiedEventHandler::windowFocus(GLFWWindow &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
}
