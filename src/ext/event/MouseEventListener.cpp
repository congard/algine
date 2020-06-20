#include <algine/ext/event/MouseEventListener.h>
#include <algine/ext/event/MouseEvent.h>
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
    m_x = x;
    m_y = y;
    
    buttons[button].isPressed = true;
    buttons[button].downX = x;
    buttons[button].downY = y;
    buttons[button].downTime = getTime();
    
    createCallback(ActionDown, button);
}

void MouseEventListener::buttonUp(const float x, const float y, const uint button) {
    m_x = x;
    m_y = y;
    
    buttons[button].isPressed = false;
    
    createCallback(ActionUp, button);
    
    if (buttons[button].canClick) {
        if (getTime() - buttons[button].downTime >= m_longClickDelay)
            createCallback(ActionLongClick, button);
        else
            createCallback(ActionClick, button);
    }
    
    buttons[button].canClick = true;
}

inline float distance(const float x1, const float x2, const float y1, const float y2) {
    return sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2));
}

void MouseEventListener::mouseMove(const float x, const float y) {
    m_x = x;
    m_y = y;
    
    for (auto & button : buttons) {
        if ((button.isPressed && button.canClick) &&
            (distance(x, button.downX, y, button.downY) > m_clickRadius))
        {
            button.canClick = false;
        }
    }
    
    createCallback(ActionMove, ButtonNone);
}

void MouseEventListener::setCallback(void (*inCallback)(MouseEvent *event)) {
    callback = inCallback;
}

void MouseEventListener::setClickRadius(const float clickRadius) {
    m_clickRadius = clickRadius;
}

void MouseEventListener::setLongClickDelay(const long longClickDelay) {
    m_longClickDelay = longClickDelay;
}

float MouseEventListener::getX() const {
    return m_x;
}

float MouseEventListener::getY() const {
    return m_y;
}

float MouseEventListener::getClickRadius() const {
    return m_clickRadius;
}

long MouseEventListener::getLongClickDelay() const {
    return m_longClickDelay;
}

MouseButton MouseEventListener::getLeftButton() const {
    return buttons[ButtonLeft];
}

MouseButton MouseEventListener::getRightButton() const {
    return buttons[ButtonRight];
}
}
