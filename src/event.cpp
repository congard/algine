#include <algine/event.h>
#include <cmath>

namespace algine {
void MouseEventListener::createCallback(const uint action, const uint button) {
    MouseEvent *event = new MouseEvent;
    event->action = action;
    event->button = button;
    event->listener = this;
    
    callback(event);
    
    delete event;
}

void MouseEventListener::buttonDown(const float x, const float y, const uint button) {
    this->x = x;
    this->y = y;
    
    buttons[button].isPressed = true;
    buttons[button].downX = x;
    buttons[button].downY = y;
    
    createCallback(actionDown, button);
}

void MouseEventListener::buttonUp(const float x, const float y, const uint button) {
    this->x = x;
    this->y = y;
    
    buttons[button].isPressed = false;
    
    createCallback(actionUp, button);
    
    if (buttons[button].canClick)
        createCallback(actionClick, button);
    
    buttons[button].canClick = true;
}

#define _distance(downX, downY) sqrt(pow(x - downX, 2) + pow(y - downY, 2))
#define _checkBtn(button) \
    if (button.isPressed && button.canClick)                     \
        if (_distance(button.downX, button.downY) > clickRadius) \
            button.canClick = false;
        
void MouseEventListener::mouseMove(const float x, const float y) {
    this->x = x;
    this->y = y;
    
    for (uint i = 0; i < buttonNone; i++) {
        _checkBtn(buttons[i]);
    }
    
    createCallback(actionMove, buttonNone);
}

void MouseEventListener::setCallback(void (*callback)(MouseEvent *event)) {
    this->callback = callback;
}
}
