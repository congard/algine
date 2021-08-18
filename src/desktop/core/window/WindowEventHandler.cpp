#include <algine/core/window/WindowEventHandler.h>

namespace algine {
// so that you don't need to implement handling of each event

void WindowEventHandler::mouseMove(double x, double y, Window &window) {}
void WindowEventHandler::mouseClick(MouseKey key, Window &window) {}
void WindowEventHandler::mouseKeyPress(MouseKey key, Window &window) {}
void WindowEventHandler::mouseKeyRelease(MouseKey key, Window &window) {}

void WindowEventHandler::keyboardKeyPress(KeyboardKey key, Window &window) {}
void WindowEventHandler::keyboardKeyRelease(KeyboardKey key, Window &window) {}
void WindowEventHandler::keyboardKeyRepeat(KeyboardKey key, Window &window) {}

void WindowEventHandler::windowSizeChange(int width, int height, Window &window) {}
void WindowEventHandler::windowPosChange(int x, int y, Window &window) {}
}
