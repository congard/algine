#include <algine/core/x/XEventHandler.h>
#include <algine/core/window/QtWindow.h>

namespace algine {
void XEventHandler::mouseMove(float x, float y, QtWindow &window) {
    onPointerMove({{x, y}, window});
}

void XEventHandler::mouseClick(MouseKey key, QtWindow &window) {
    onPointerClick({window.getCursorPos(), key, window});
}

void XEventHandler::mouseKeyPress(MouseKey key, QtWindow &window) {
    onPointerPress({window.getCursorPos(), key, window});
}

void XEventHandler::mouseKeyRelease(MouseKey key, QtWindow &window) {
    onPointerRelease({window.getCursorPos(), key, window});
}

void XEventHandler::keyboardKeyPress(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyPress(key);
}

void XEventHandler::keyboardKeyRelease(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRelease(key);
}

void XEventHandler::keyboardKeyRepeat(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRepeat(key);
}

void XEventHandler::windowSizeChange(int width, int height, QtWindow &window) {
    onSizeChanged(width, height);
}

void XEventHandler::windowIconify(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void XEventHandler::windowRestore(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void XEventHandler::windowFocusLost(QtWindow &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void XEventHandler::windowFocus(QtWindow &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
}
