#ifndef ALGINE_WINDOWEVENTHANDLER_H
#define ALGINE_WINDOWEVENTHANDLER_H

#include <algine/core/EventHandler.h>

#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>

namespace algine {
class GLFWWindow;

class WindowEventHandler: public EventHandler {
public:
    virtual void mouseMove(double x, double y, GLFWWindow &window);
    virtual void mouseClick(MouseKey key, GLFWWindow &window);
    virtual void mouseKeyPress(MouseKey key, GLFWWindow &window);
    virtual void mouseKeyRelease(MouseKey key, GLFWWindow &window);

    virtual void keyboardKeyPress(KeyboardKey key, GLFWWindow &window);
    virtual void keyboardKeyRelease(KeyboardKey key, GLFWWindow &window);
    virtual void keyboardKeyRepeat(KeyboardKey key, GLFWWindow &window);

    virtual void windowSizeChange(int width, int height, GLFWWindow &window);
    virtual void windowPosChange(int x, int y, GLFWWindow &window);
    virtual void windowIconify(GLFWWindow &window);
    virtual void windowRestore(GLFWWindow &window);
    virtual void windowFocusLost(GLFWWindow &window);
    virtual void windowFocus(GLFWWindow &window);
};
}

#endif //ALGINE_WINDOWEVENTHANDLER_H
