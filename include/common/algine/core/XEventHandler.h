#ifndef ALGINE_XEVENTHANDLER_H
#define ALGINE_XEVENTHANDLER_H

#ifdef __ANDROID__
    #include <algine/core/ScreenEventHandler.h>
#else
    #include <algine/core/window/WindowEventHandler.h>
#endif

#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>
#include <algine/templates.h>

#include <glm/vec2.hpp>

#include <variant>

namespace algine {
/**
 * Platform independent EventHandler
 */
class XEventHandler: public
        enable_if_desktop(WindowEventHandler)
        enable_if_android(ScreenEventHandler) {
public:
    class PointerInfo;
    class FocusInfo;

public:
    virtual void onPointerMove(PointerInfo info);
    virtual void onPointerClick(PointerInfo info);
    virtual void onPointerPress(PointerInfo info);
    virtual void onPointerRelease(PointerInfo info);

    virtual void onKeyboardKeyPress(KeyboardKey key);
    virtual void onKeyboardKeyRelease(KeyboardKey key);
    virtual void onKeyboardKeyRepeat(KeyboardKey key);

    virtual void onSizeChanged(int width, int height);

    virtual void onFocusChanged(FocusInfo info);

#ifdef __ANDROID__
    void pointerDown(float x, float y, int pointerId) override;
    void pointerMove(float x, float y, int pointerId) override;
    void pointerUp(float x, float y, int pointerId) override;
    void pointerClick(float x, float y, int pointerId) override;

    void surfaceResized(int width, int height) override;

    void onPause() override;
    void onResume() override;
#else
    void mouseMove(double x, double y, Window &window) override;
    void mouseClick(MouseKey key, Window &window) override;
    void mouseKeyPress(MouseKey key, Window &window) override;
    void mouseKeyRelease(MouseKey key, Window &window) override;

    void keyboardKeyPress(KeyboardKey key, Window &window) override;
    void keyboardKeyRelease(KeyboardKey key, Window &window) override;
    void keyboardKeyRepeat(KeyboardKey key, Window &window) override;

    void windowSizeChange(int width, int height, Window &window) override;
    void windowIconify(Window &window) override;
    void windowRestore(Window &window) override;
    void windowFocusLost(Window &window) override;
    void windowFocus(Window &window) override;
#endif
};

class XEventHandler::PointerInfo {
public:
    using Finger = int;
    using Pointer = std::variant<MouseKey, Finger>;

public:
#ifdef __ANDROID__
    inline PointerInfo(float x, float y)
        : m_pos(x, y) {}

    template<typename T>
    PointerInfo(float x, float y, T pointer)
        : m_pos(x, y), m_pointer(pointer) {}
#else
    inline PointerInfo(const glm::vec2 &pos, Window &window)
        : m_pos(pos), m_window(window) {}

    template<typename T>
    PointerInfo(const glm::vec2 &pos, T pointer, Window &window)
        : m_pos(pos), m_pointer(pointer), m_window(window) {}

    inline Window& getWindow() { return m_window; }
#endif

    void setPos(const glm::vec2 &pos);
    const glm::vec2& getPos() const;

    void setX(float x);
    void setY(float y);
    float getX() const;
    float getY() const;

    void setMouseKey(MouseKey key);
    void setFinger(Finger id);
    bool isMouseKey() const;
    bool isFinger() const;
    MouseKey getMouseKey() const;
    Finger getFinger() const;
    const Pointer& getPointer() const;

    bool isPointerActive(Pointer pointer) const;
    glm::vec2 getPointerPos(Pointer pointer) const;

private:
    glm::vec2 m_pos;
    Pointer m_pointer;

#ifndef __ANDROID__
    Window &m_window;
#endif
};

inline void XEventHandler::PointerInfo::setPos(const glm::vec2 &pos) {
    m_pos = pos;
}

inline const glm::vec2& XEventHandler::PointerInfo::getPos() const {
    return m_pos;
}

inline void XEventHandler::PointerInfo::setX(float x) {
    m_pos.x = x;
}

inline void XEventHandler::PointerInfo::setY(float y) {
    m_pos.y = y;
}

inline float XEventHandler::PointerInfo::getX() const {
    return m_pos.x;
}

inline float XEventHandler::PointerInfo::getY() const {
    return m_pos.y;
}

inline void XEventHandler::PointerInfo::setMouseKey(MouseKey key) {
    m_pointer = key;
}

inline void XEventHandler::PointerInfo::setFinger(Finger id) {
    m_pointer = id;
}

inline bool XEventHandler::PointerInfo::isMouseKey() const {
    return std::get_if<MouseKey>(&m_pointer);
}

inline bool XEventHandler::PointerInfo::isFinger() const {
    return std::get_if<Finger>(&m_pointer);
}

inline MouseKey XEventHandler::PointerInfo::getMouseKey() const {
    return std::get<MouseKey>(m_pointer);
}

inline XEventHandler::PointerInfo::Finger XEventHandler::PointerInfo::getFinger() const {
    return std::get<Finger>(m_pointer);
}

inline const XEventHandler::PointerInfo::Pointer& XEventHandler::PointerInfo::getPointer() const {
    return m_pointer;
}

class XEventHandler::FocusInfo {
public:
    enum Reason {
        Unknown,
        Focused,
        FocusLost,
        WindowIconified,
        WindowRestored,
        AppPaused,
        AppResumed
    };

public:
#ifdef __ANDROID__
    inline FocusInfo(Reason reason)
        : m_reason(reason) {}
#else
    inline FocusInfo(Reason reason, Window &window)
        : m_reason(reason), m_window(window) {}

    inline Window& getWindow() { return m_window; }
#endif

    void setReason(Reason reason);
    Reason getReason() const;

private:
    Reason m_reason {Reason::Unknown};

#ifndef __ANDROID__
    Window &m_window;
#endif
};

inline void XEventHandler::FocusInfo::setReason(Reason reason) {
    m_reason = reason;
}

inline XEventHandler::FocusInfo::Reason XEventHandler::FocusInfo::getReason() const {
    return m_reason;
}
}

#endif //ALGINE_XEVENTHANDLER_H
