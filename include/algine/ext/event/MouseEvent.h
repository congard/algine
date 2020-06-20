#ifndef ALGINE_MOUSEEVENT_H
#define ALGINE_MOUSEEVENT_H

#include <algine/ext/event/MouseEventListener.h>

namespace algine {
class MouseEvent {
public:
    float getX() const;
    float getY() const;

public:
    MouseEventListener *listener;
    uint action;
    uint button;
};
}

#endif //ALGINE_MOUSEEVENT_H
