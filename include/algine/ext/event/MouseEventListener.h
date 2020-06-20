#ifndef ALGINE_MOUSEEVENTLISTENER_H
#define ALGINE_MOUSEEVENTLISTENER_H

#include <algine/types.h>
#include <algine/ext/event/MouseButton.h>

namespace algine {
class MouseEvent;

class MouseEventListener {
    friend class MouseEvent;

public:
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

public:
    void buttonDown(float x, float y, uint button = ButtonLeft);
    void buttonUp(float x, float y, uint button = ButtonLeft);
    void mouseMove(float x, float y);

    void setCallback(void (*inCallback)(MouseEvent *event));
    void setClickRadius(float clickRadius);
    void setLongClickDelay(long longClickDelay);

    float getX() const;
    float getY() const;
    float getClickRadius() const;
    long getLongClickDelay() const;
    MouseButton getLeftButton() const;
    MouseButton getRightButton() const;

protected:
    float m_x = -1, m_y = -1;
    float m_clickRadius = 2.0f;
    long m_longClickDelay = 2000; // in ms
    MouseButton buttons[ButtonNone];

private:
    void (*callback)(MouseEvent *event) = nullptr;
    
    void createCallback(uint action, uint button);
};
}

#endif
