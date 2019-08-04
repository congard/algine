#ifndef ALGINE_EVENT_H
#define ALGINE_EVENT_H

#include <algine/types.h>

namespace algine {
struct MouseEventListener {
    enum {
        ActionDown,
        ActionMove,
        ActionUp,
        ActionClick,
        ActionLongClick
    };
    
    enum {
        ButtonLeft,
        ButtonRight,
        ButtonNone
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
        long downTime;
        bool isPressed = false;
        bool canClick = true;
    } buttons[ButtonNone];
    
    float x, y;
    float clickRadius = 2.0f;
    long longClickTime = 2000; // in ms
    
    void buttonDown(const float x, const float y, const uint button = ButtonLeft);
    void buttonUp(const float x, const float y, const uint button = ButtonLeft);
    void mouseMove(const float x, const float y);
    void setCallback(void (*callback)(MouseEvent *event));
    
private:
    void (*callback)(MouseEvent *event);
    
    void createCallback(const uint action, const uint button);
};
}

#endif
