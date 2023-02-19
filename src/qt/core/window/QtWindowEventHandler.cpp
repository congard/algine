#include <algine/core/window/QtWindowEventHandler.h>

namespace algine {
void QtWindowEventHandler::mouseMove(float x, float y, QtWindow &window) {}
void QtWindowEventHandler::mouseClick(MouseKey key, QtWindow &window) {}
void QtWindowEventHandler::mouseKeyPress(MouseKey key, QtWindow &window) {}
void QtWindowEventHandler::mouseKeyRelease(MouseKey key, QtWindow &window) {}

void QtWindowEventHandler::keyboardKeyPress(KeyboardKey key, QtWindow &window) {}
void QtWindowEventHandler::keyboardKeyRelease(KeyboardKey key, QtWindow &window) {}
void QtWindowEventHandler::keyboardKeyRepeat(KeyboardKey key, QtWindow &window) {}

void QtWindowEventHandler::windowSizeChange(int width, int height, QtWindow &window) {}
void QtWindowEventHandler::windowPosChange(int x, int y, QtWindow &window) {}
void QtWindowEventHandler::windowIconify(QtWindow &window) {}
void QtWindowEventHandler::windowRestore(QtWindow &window) {}
void QtWindowEventHandler::windowFocusLost(QtWindow &window) {}
void QtWindowEventHandler::windowFocus(QtWindow &window) {}
} // algine
