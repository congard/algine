#include <algine/core/unified/UnifiedEventHandler.h>
#include <algine/core/window/QtWindow.h>

namespace algine {
void UnifiedEventHandler::mouseMove(float x, float y, QtWindow &window) {
    onPointerMove({{x, y}, window});
}

void UnifiedEventHandler::mouseClick(MouseKey key, QtWindow &window) {
    onPointerClick({window.getCursorPos(), key, window});
}

void UnifiedEventHandler::mouseKeyPress(MouseKey key, QtWindow &window) {
    onPointerPress({window.getCursorPos(), key, window});
}

void UnifiedEventHandler::mouseKeyRelease(MouseKey key, QtWindow &window) {
    onPointerRelease({window.getCursorPos(), key, window});
}

void UnifiedEventHandler::keyboardKeyPress(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyPress(key);
}

void UnifiedEventHandler::keyboardKeyRelease(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRelease(key);
}

void UnifiedEventHandler::keyboardKeyRepeat(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRepeat(key);
}

void UnifiedEventHandler::windowSizeChange(int width, int height, QtWindow &window) {
    onSizeChanged(width, height);
}

void UnifiedEventHandler::windowIconify(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void UnifiedEventHandler::windowRestore(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void UnifiedEventHandler::windowFocusLost(QtWindow &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void UnifiedEventHandler::windowFocus(QtWindow &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
}
