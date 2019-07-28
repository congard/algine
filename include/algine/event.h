#ifndef ALGINE_EVENT_H
#define ALGINE_EVENT_H

#include <algine/types.h>

namespace algine {
struct MouseEventListener {
    enum {
        actionDown,
        actionMove,
        actionUp,
        actionClick,
        actionDoubleClick
    };
    
    enum {
        buttonLeft,
        buttonRight,
        buttonNone
    };
    
    struct MouseEvent {
        MouseEventListener *listener;
        uint action;
        uint button;
        
        inline float getX() { return listener->x; };
        inline float getY() { return listener->y; };
    };
    
    struct MouseButton {
        float downX, downY;
        bool isPressed = false;
        bool canClick = true;
    } buttons[buttonNone];
    
    float x, y;
    float clickRadius = 2.0f;
    
    void buttonDown(const float x, const float y, const uint button = buttonLeft);
    void buttonUp(const float x, const float y, const uint button = buttonLeft);
    void mouseMove(const float x, const float y);
    void setCallback(void (*callback)(MouseEvent *event));
    
private:
    void (*callback)(MouseEvent *event);
    
    void createCallback(const uint action, const uint button);
};
}

#endif
