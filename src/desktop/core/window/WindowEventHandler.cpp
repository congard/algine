#include <algine/core/window/WindowEventHandler.h>

namespace algine {
// so that you don't need to implement handling of each event

void WindowEventHandler::mouseMove(double x, double y) {}
void WindowEventHandler::mouseClick(MouseKey key) {}
void WindowEventHandler::mouseKeyPress(MouseKey key) {}
void WindowEventHandler::mouseKeyRelease(MouseKey key) {}

void WindowEventHandler::keyboardKeyPress(KeyboardKey key) {}
void WindowEventHandler::keyboardKeyRelease(KeyboardKey key) {}
void WindowEventHandler::keyboardKeyRepeat(KeyboardKey key) {}

void WindowEventHandler::windowSizeChange(int width, int height) {}
void WindowEventHandler::windowPosChange(int x, int y) {}
}
