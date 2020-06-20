#ifndef ALGINE_MOUSEBUTTON_H
#define ALGINE_MOUSEBUTTON_H

namespace algine {
class MouseButton {
public:
    float downX = -1, downY = -1;
    long downTime = -1;
    bool isPressed = false;
    bool canClick = true;
};
}

#endif //ALGINE_MOUSEBUTTON_H
