#ifndef ALGINE_QTWIDGETEVENTHANDLER_H
#define ALGINE_QTWIDGETEVENTHANDLER_H

#include <algine/core/EventHandler.h>
#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>

namespace algine {
class QtWindow;

class AL_EXPORT QtWindowEventHandler: public EventHandler {
public:
    virtual void mouseMove(float x, float y, QtWindow &window);
    virtual void mouseClick(MouseKey key, QtWindow &window);
    virtual void mouseKeyPress(MouseKey key, QtWindow &window);
    virtual void mouseKeyRelease(MouseKey key, QtWindow &window);

    virtual void keyboardKeyPress(KeyboardKey key, QtWindow &window);
    virtual void keyboardKeyRelease(KeyboardKey key, QtWindow &window);
    virtual void keyboardKeyRepeat(KeyboardKey key, QtWindow &window);

    virtual void windowSizeChange(int width, int height, QtWindow &window);
    virtual void windowPosChange(int x, int y, QtWindow &window);
    virtual void windowIconify(QtWindow &window);
    virtual void windowRestore(QtWindow &window);
    virtual void windowFocusLost(QtWindow &window);
    virtual void windowFocus(QtWindow &window);
};
} // algine

#endif //ALGINE_QTWIDGETEVENTHANDLER_H
