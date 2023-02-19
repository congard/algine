#ifndef ALGINE_TESTS_MOUSEKEYCONVERTER_H
#define ALGINE_TESTS_MOUSEKEYCONVERTER_H

#include <algine/core/input/MouseKey.h>

#include <qnamespace.h>

// TODO: add other buttons

namespace algine {
inline MouseKey getMouseKey(Qt::MouseButton button) {
    switch (button) {
        case Qt::NoButton: return MouseKey::Unknown;
        case Qt::LeftButton: return MouseKey::Left;
        case Qt::RightButton: return MouseKey::Right;
        case Qt::MiddleButton: return MouseKey::Middle;
        default: return MouseKey::Unknown;
    }
}

inline Qt::MouseButton getQtMouseButton(MouseKey key) {
    switch (key) {
        case MouseKey::Unknown: return Qt::NoButton;
        case MouseKey::Left: return Qt::LeftButton;
        case MouseKey::Right: return Qt::RightButton;
        case MouseKey::Middle: return Qt::MiddleButton;
        default: return Qt::NoButton;
    }
}
}

#endif //ALGINE_TESTS_MOUSEKEYCONVERTER_H
