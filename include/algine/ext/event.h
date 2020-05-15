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
        
        inline float getX() const { return listener->x; };
        inline float getY() const { return listener->y; };
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
    
    void buttonDown(float x, float y, uint button = ButtonLeft);
    void buttonUp(float x, float y, uint button = ButtonLeft);
    void mouseMove(float x, float y);
    void setCallback(void (*callback)(MouseEvent *event));
    
private:
    void (*callback)(MouseEvent *event);
    
    void createCallback(uint action, uint button);
};
}

#endif
