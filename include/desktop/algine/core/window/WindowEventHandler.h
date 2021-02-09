#ifndef ALGINE_WINDOWEVENTHANDLER_H
#define ALGINE_WINDOWEVENTHANDLER_H

#include <algine/core/EventHandler.h>

#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>

namespace algine {
class WindowEventHandler: public EventHandler {
public:
    virtual void mouseMove(double x, double y);
    virtual void mouseClick(MouseKey key);
    virtual void mouseKeyPress(MouseKey key);
    virtual void mouseKeyRelease(MouseKey key);

    virtual void keyboardKeyPress(KeyboardKey key);
    virtual void keyboardKeyRelease(KeyboardKey key);
    virtual void keyboardKeyRepeat(KeyboardKey key);

    virtual void windowSizeChange(int width, int height);
    virtual void windowPosChange(int x, int y);
};
}

#endif //ALGINE_WINDOWEVENTHANDLER_H
