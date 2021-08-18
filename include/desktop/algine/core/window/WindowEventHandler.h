#ifndef ALGINE_WINDOWEVENTHANDLER_H
#define ALGINE_WINDOWEVENTHANDLER_H

#include <algine/core/EventHandler.h>

#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>

namespace algine {
class Window;

class WindowEventHandler: public EventHandler {
public:
    virtual void mouseMove(double x, double y, Window &window);
    virtual void mouseClick(MouseKey key, Window &window);
    virtual void mouseKeyPress(MouseKey key, Window &window);
    virtual void mouseKeyRelease(MouseKey key, Window &window);

    virtual void keyboardKeyPress(KeyboardKey key, Window &window);
    virtual void keyboardKeyRelease(KeyboardKey key, Window &window);
    virtual void keyboardKeyRepeat(KeyboardKey key, Window &window);

    virtual void windowSizeChange(int width, int height, Window &window);
    virtual void windowPosChange(int x, int y, Window &window);
};
}

#endif //ALGINE_WINDOWEVENTHANDLER_H
