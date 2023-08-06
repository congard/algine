#include <algine/core/window/WindowEventHandler.h>

namespace algine {
// so that you don't need to implement handling of each event

void WindowEventHandler::mouseMove(double x, double y, GLFWWindow &window) {}
void WindowEventHandler::mouseClick(MouseKey key, GLFWWindow &window) {}
void WindowEventHandler::mouseKeyPress(MouseKey key, GLFWWindow &window) {}
void WindowEventHandler::mouseKeyRelease(MouseKey key, GLFWWindow &window) {}

void WindowEventHandler::keyboardKeyPress(KeyboardKey key, GLFWWindow &window) {}
void WindowEventHandler::keyboardKeyRelease(KeyboardKey key, GLFWWindow &window) {}
void WindowEventHandler::keyboardKeyRepeat(KeyboardKey key, GLFWWindow &window) {}

void WindowEventHandler::windowSizeChange(int width, int height, GLFWWindow &window) {}
void WindowEventHandler::windowPosChange(int x, int y, GLFWWindow &window) {}
void WindowEventHandler::windowIconify(GLFWWindow &window) {}
void WindowEventHandler::windowRestore(GLFWWindow &window) {}
void WindowEventHandler::windowFocusLost(GLFWWindow &window) {}
void WindowEventHandler::windowFocus(GLFWWindow &window) {}
}
