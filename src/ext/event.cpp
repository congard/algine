#include <algine/ext/event.h>
#include <cmath>
#include <chrono>

using namespace std;

inline long getTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

namespace algine {
void MouseEventListener::createCallback(const uint action, const uint button) {
    auto event = new MouseEvent;
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
    buttons[button].downTime = getTime();
    
    createCallback(ActionDown, button);
}

void MouseEventListener::buttonUp(const float x, const float y, const uint button) {
    this->x = x;
    this->y = y;
    
    buttons[button].isPressed = false;
    
    createCallback(ActionUp, button);
    
    if (buttons[button].canClick) {
        if (getTime() - buttons[button].downTime >= longClickTime)
            createCallback(ActionLongClick, button);
        else
            createCallback(ActionClick, button);
    }
    
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
    
    for (uint i = 0; i < ButtonNone; i++) {
        _checkBtn(buttons[i]);
    }
    
    createCallback(ActionMove, ButtonNone);
}

void MouseEventListener::setCallback(void (*callback)(MouseEvent *event)) {
    this->callback = callback;
}
}
