#include <algine/ext/event/MouseEvent.h>

namespace algine {
float MouseEvent::getX() const {
    return listener->m_x;
}

float MouseEvent::getY() const {
    return listener->m_y;
}
}
