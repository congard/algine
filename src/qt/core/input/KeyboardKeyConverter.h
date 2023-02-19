#ifndef ALGINE_TESTS_KEYBOARDKEYCONVERTER_H
#define ALGINE_TESTS_KEYBOARDKEYCONVERTER_H

#include <algine/core/input/KeyboardKey.h>

#include <QKeyEvent>

namespace algine {
inline KeyboardKey getKeyboardKey(QKeyEvent *event) {
    if (event->modifiers() & Qt::KeypadModifier) {
        switch (event->key()) {
            case Qt::Key_0: return KeyboardKey::KeyPad0;
            case Qt::Key_1: return KeyboardKey::KeyPad1;
            case Qt::Key_2: return KeyboardKey::KeyPad2;
            case Qt::Key_3: return KeyboardKey::KeyPad3;
            case Qt::Key_4: return KeyboardKey::KeyPad4;
            case Qt::Key_5: return KeyboardKey::KeyPad5;
            case Qt::Key_6: return KeyboardKey::KeyPad6;
            case Qt::Key_7: return KeyboardKey::KeyPad7;
            case Qt::Key_8: return KeyboardKey::KeyPad8;
            case Qt::Key_9: return KeyboardKey::KeyPad9;
            case Qt::Key_Period: return KeyboardKey::KeyPadDecimal;
            case Qt::Key_Slash: return KeyboardKey::KeyPadDivide;
            case Qt::Key_Asterisk: return KeyboardKey::KeyPadMultiply;
            case Qt::Key_Minus: return KeyboardKey::KeyPadSubtract;
            case Qt::Key_Plus: return KeyboardKey::KeyPadAdd;
            case Qt::Key_Enter: return KeyboardKey::KeyPadEnter;
            case Qt::Key_Equal: return KeyboardKey::KeyPadEqual;
        }
    }

    // Note: below mapping doesn't take into account modifiers

#ifdef __linux__
    // Note: X11 scancodes
    constexpr auto LEFT_SHIFT = 50;
    constexpr auto LEFT_CTRL = 37;
    constexpr auto LEFT_ALT = 64;
    constexpr auto LEFT_META = 133;
#else
    constexpr auto LEFT_SHIFT = 0xA0;
    constexpr auto LEFT_CTRL = 0xA2;
    constexpr auto LEFT_ALT = 0xA4;
    constexpr auto LEFT_META = 0x5B;
#endif

    auto scancode = event->nativeScanCode();

    switch (event->key()) {
        case Qt::Key_unknown: return KeyboardKey::Unknown;
        case Qt::Key_Space: return KeyboardKey::Space;
        case Qt::Key_Apostrophe: return KeyboardKey::Apostrophe;
        case Qt::Key_Comma: return KeyboardKey::Comma;
        case Qt::Key_Minus: return KeyboardKey::Minus;
        case Qt::Key_Period: return KeyboardKey::Dot;
        case Qt::Key_Slash: return KeyboardKey::Slash;
        case Qt::Key_0: return KeyboardKey::Key0;
        case Qt::Key_1: return KeyboardKey::Key1;
        case Qt::Key_2: return KeyboardKey::Key2;
        case Qt::Key_3: return KeyboardKey::Key3;
        case Qt::Key_4: return KeyboardKey::Key4;
        case Qt::Key_5: return KeyboardKey::Key5;
        case Qt::Key_6: return KeyboardKey::Key6;
        case Qt::Key_7: return KeyboardKey::Key7;
        case Qt::Key_8: return KeyboardKey::Key8;
        case Qt::Key_9: return KeyboardKey::Key9;
        case Qt::Key_Semicolon: return KeyboardKey::Semicolon;
        case Qt::Key_Equal: return KeyboardKey::Equal;
        case Qt::Key_A: return KeyboardKey::A;
        case Qt::Key_B: return KeyboardKey::B;
        case Qt::Key_C: return KeyboardKey::C;
        case Qt::Key_D: return KeyboardKey::D;
        case Qt::Key_E: return KeyboardKey::E;
        case Qt::Key_F: return KeyboardKey::F;
        case Qt::Key_G: return KeyboardKey::G;
        case Qt::Key_H: return KeyboardKey::H;
        case Qt::Key_I: return KeyboardKey::I;
        case Qt::Key_J: return KeyboardKey::J;
        case Qt::Key_K: return KeyboardKey::K;
        case Qt::Key_L: return KeyboardKey::L;
        case Qt::Key_M: return KeyboardKey::M;
        case Qt::Key_N: return KeyboardKey::N;
        case Qt::Key_O: return KeyboardKey::O;
        case Qt::Key_P: return KeyboardKey::P;
        case Qt::Key_Q: return KeyboardKey::Q;
        case Qt::Key_R: return KeyboardKey::R;
        case Qt::Key_S: return KeyboardKey::S;
        case Qt::Key_T: return KeyboardKey::T;
        case Qt::Key_U: return KeyboardKey::U;
        case Qt::Key_V: return KeyboardKey::V;
        case Qt::Key_W: return KeyboardKey::W;
        case Qt::Key_X: return KeyboardKey::X;
        case Qt::Key_Y: return KeyboardKey::Y;
        case Qt::Key_Z: return KeyboardKey::Z;
        case Qt::Key_BracketLeft: return KeyboardKey::LeftBracket;
        case Qt::Key_Backslash: return KeyboardKey::Backslash;
        case Qt::Key_BracketRight: return KeyboardKey::RightBracket;
        case Qt::Key_QuoteLeft: return KeyboardKey::GraveAccent;
        case Qt::Key_Escape: return KeyboardKey::Escape;
        case Qt::Key_Enter: return KeyboardKey::Enter;
        case Qt::Key_Tab: return KeyboardKey::Tab;
        case Qt::Key_Backspace: return KeyboardKey::Backspace;
        case Qt::Key_Insert: return KeyboardKey::Insert;
        case Qt::Key_Delete: return KeyboardKey::Delete;
        case Qt::Key_Right: return KeyboardKey::Right;
        case Qt::Key_Left: return KeyboardKey::Left;
        case Qt::Key_Down: return KeyboardKey::Down;
        case Qt::Key_Up: return KeyboardKey::Up;
        case Qt::Key_PageUp: return KeyboardKey::PageUp;
        case Qt::Key_PageDown: return KeyboardKey::PageDown;
        case Qt::Key_Home: return KeyboardKey::Home;
        case Qt::Key_End: return KeyboardKey::End;
        case Qt::Key_CapsLock: return KeyboardKey::CapsLock;
        case Qt::Key_ScrollLock: return KeyboardKey::ScrollLock;
        case Qt::Key_NumLock: return KeyboardKey::NumLock;
        case Qt::Key_Print: return KeyboardKey::PrintScreen;
        case Qt::Key_Pause: return KeyboardKey::Pause;
        case Qt::Key_F1: return KeyboardKey::F1;
        case Qt::Key_F2: return KeyboardKey::F2;
        case Qt::Key_F3: return KeyboardKey::F3;
        case Qt::Key_F4: return KeyboardKey::F4;
        case Qt::Key_F5: return KeyboardKey::F5;
        case Qt::Key_F6: return KeyboardKey::F6;
        case Qt::Key_F7: return KeyboardKey::F7;
        case Qt::Key_F8: return KeyboardKey::F8;
        case Qt::Key_F9: return KeyboardKey::F9;
        case Qt::Key_F10: return KeyboardKey::F10;
        case Qt::Key_F11: return KeyboardKey::F11;
        case Qt::Key_F12: return KeyboardKey::F12;
        case Qt::Key_F13: return KeyboardKey::F13;
        case Qt::Key_F14: return KeyboardKey::F14;
        case Qt::Key_F15: return KeyboardKey::F15;
        case Qt::Key_F16: return KeyboardKey::F16;
        case Qt::Key_F17: return KeyboardKey::F17;
        case Qt::Key_F18: return KeyboardKey::F18;
        case Qt::Key_F19: return KeyboardKey::F19;
        case Qt::Key_F20: return KeyboardKey::F20;
        case Qt::Key_F21: return KeyboardKey::F21;
        case Qt::Key_F22: return KeyboardKey::F22;
        case Qt::Key_F23: return KeyboardKey::F23;
        case Qt::Key_F24: return KeyboardKey::F24;
        case Qt::Key_F25: return KeyboardKey::F25;
        case Qt::Key_Shift:
            return scancode == LEFT_SHIFT ? KeyboardKey::LeftShift : KeyboardKey::RightShift;
        case Qt::Key_Control:
            return scancode == LEFT_CTRL ? KeyboardKey::LeftControl : KeyboardKey::RightControl;
        case Qt::Key_Alt:
            return scancode == LEFT_ALT ? KeyboardKey::LeftAlt : KeyboardKey::RightAlt;
        case Qt::Key_Meta:
            return scancode == LEFT_META ? KeyboardKey::LeftSuper : KeyboardKey::RightSuper;
        default: return KeyboardKey::Unknown;
    }
}
}

#endif //ALGINE_TESTS_KEYBOARDKEYCONVERTER_H
