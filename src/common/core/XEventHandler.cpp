#include <algine/core/XEventHandler.h>
#include <algine/core/math/Size.h>
#include <algine/core/Event.h>

#ifdef __ANDROID__
    #include <algine/core/Screen.h>
#elif defined(ALGINE_QT_PLATFORM)
    #include <algine/core/window/QtWindow.h>
#else
    #include <algine/core/window/Window.h>
#endif

namespace algine {
template<typename T>
inline void pushEvent(Event::Id id, const T &data,
                      std::forward_list<XEventHandler::EventListener> &listeners)
{
    Event event(id, data);

    for (auto &listener : listeners) {
        listener(event);
    }
}

void XEventHandler::onPointerMove(PointerInfo info) {
    pushEvent(Event::Move, info, m_pointerListeners);
}

void XEventHandler::onPointerClick(PointerInfo info) {
    pushEvent(Event::Click, info, m_pointerListeners);
}

void XEventHandler::onPointerPress(PointerInfo info) {
    pushEvent(Event::Press, info, m_pointerListeners);
}

void XEventHandler::onPointerRelease(PointerInfo info) {
    pushEvent(Event::Release, info, m_pointerListeners);
}

void XEventHandler::onKeyboardKeyPress(KeyboardKey key) {
    pushEvent(Event::KeyPress, key, m_keyboardListeners);
}

void XEventHandler::onKeyboardKeyRelease(KeyboardKey key) {
    pushEvent(Event::KeyRelease, key, m_keyboardListeners);
}

void XEventHandler::onKeyboardKeyRepeat(KeyboardKey key) {
    pushEvent(Event::KeyRepeat, key, m_keyboardListeners);
}

void XEventHandler::onSizeChanged(int width, int height) {
    pushEvent(Event::SurfaceSizeChange, SizeI(width, height), m_surfaceListeners);
}

void XEventHandler::onFocusChanged(FocusInfo info) {
    pushEvent(Event::SurfaceFocusChange, info, m_surfaceListeners);
}

void XEventHandler::addPointerEventListener(const EventListener &listener) {
    m_pointerListeners.emplace_front(listener);
}

void XEventHandler::addKeyboardEventListener(const EventListener &listener) {
    m_keyboardListeners.emplace_front(listener);
}

void XEventHandler::addSurfaceEventListener(const EventListener &listener) {
    m_surfaceListeners.emplace_front(listener);
}

void XEventHandler::clearPointerEventListeners() {
    m_pointerListeners.clear();
}

void XEventHandler::clearKeyboardEventListeners() {
    m_keyboardListeners.clear();
}

void XEventHandler::clearSurfaceEventListeners() {
    m_surfaceListeners.clear();
}

#ifdef __ANDROID__
bool XEventHandler::PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return false;
    } else {
        return Screen::instance()->isPointerAvailable(std::get<int>(pointer));
    }
}

glm::vec2 XEventHandler::PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<MouseKey>(&pointer) != nullptr) { // MouseKey pointer is not available on android as for now
        return {-1.0f, -1.0f};
    } else {
        return Screen::instance()->getPointerPos(std::get<int>(pointer));
    }
}

void XEventHandler::pointerDown(float x, float y, int pointerId) {
    onPointerPress({x, y, pointerId});
}

void XEventHandler::pointerMove(float x, float y, int pointerId) {
    onPointerMove({x, y, pointerId});
}

void XEventHandler::pointerUp(float x, float y, int pointerId) {
    onPointerRelease({x, y, pointerId});
}

void XEventHandler::pointerClick(float x, float y, int pointerId) {
    onPointerClick({x, y, pointerId});
}

void XEventHandler::surfaceResized(int width, int height) {
    onSizeChanged(width, height);
}

void XEventHandler::onPause() {
    onFocusChanged({FocusInfo::Reason::AppPaused});
}

void XEventHandler::onResume() {
    onFocusChanged({FocusInfo::Reason::AppResumed});
}
#elif defined(ALGINE_QT_PLATFORM)
bool XEventHandler::PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<Finger>(&pointer) != nullptr) { // not available on Qt as for now
        return false;
    } else {
        return m_window.isMouseKeyPressed(std::get<MouseKey>(pointer));
    }
}

glm::vec2 XEventHandler::PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<Finger>(&pointer) != nullptr) { // not available on Qt as for now
        return {-1.0f, -1.0f};
    } else {
        return m_pos;
    }
}

void XEventHandler::mouseMove(float x, float y, QtWindow &window) {
    onPointerMove({{x, y}, window});
}

void XEventHandler::mouseClick(MouseKey key, QtWindow &window) {
    onPointerClick({window.getCursorPos(), key, window});
}

void XEventHandler::mouseKeyPress(MouseKey key, QtWindow &window) {
    onPointerPress({window.getCursorPos(), key, window});
}

void XEventHandler::mouseKeyRelease(MouseKey key, QtWindow &window) {
    onPointerRelease({window.getCursorPos(), key, window});
}

void XEventHandler::keyboardKeyPress(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyPress(key);
}

void XEventHandler::keyboardKeyRelease(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRelease(key);
}

void XEventHandler::keyboardKeyRepeat(KeyboardKey key, QtWindow &window) {
    onKeyboardKeyRepeat(key);
}

void XEventHandler::windowSizeChange(int width, int height, QtWindow &window) {
    onSizeChanged(width, height);
}

void XEventHandler::windowIconify(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void XEventHandler::windowRestore(QtWindow &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void XEventHandler::windowFocusLost(QtWindow &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void XEventHandler::windowFocus(QtWindow &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
#else
bool XEventHandler::PointerInfo::isPointerActive(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return false;
    } else {
        return m_window.isMouseKeyPressed(std::get<MouseKey>(pointer));
    }
}

glm::vec2 XEventHandler::PointerInfo::getPointerPos(Pointer pointer) const {
    if (std::get_if<int>(&pointer) != nullptr) { // finger, not available on desktop as for now
        return {-1.0f, -1.0f};
    } else {
        return m_pos;
    }
}

inline glm::vec2 toVec2(const glm::dvec2 &v) {
    return {
        static_cast<float>(v.x),
        static_cast<float>(v.y)
    };
}

void XEventHandler::mouseMove(double x, double y, Window &window) {
    onPointerMove({
        {static_cast<float>(x), static_cast<float>(y)},
        window
    });
}

void XEventHandler::mouseClick(MouseKey key, Window &window) {
    onPointerClick({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::mouseKeyPress(MouseKey key, Window &window) {
    onPointerPress({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::mouseKeyRelease(MouseKey key, Window &window) {
    onPointerRelease({toVec2(window.getCursorPos()), key, window});
}

void XEventHandler::keyboardKeyPress(KeyboardKey key, Window &window) {
    onKeyboardKeyPress(key);
}

void XEventHandler::keyboardKeyRelease(KeyboardKey key, Window &window) {
    onKeyboardKeyRelease(key);
}

void XEventHandler::keyboardKeyRepeat(KeyboardKey key, Window &window) {
    onKeyboardKeyRepeat(key);
}

void XEventHandler::windowSizeChange(int width, int height, Window &window) {
    onSizeChanged(width, height);
}

void XEventHandler::windowIconify(Window &window) {
    onFocusChanged({FocusInfo::WindowIconified, window});
}

void XEventHandler::windowRestore(Window &window) {
    onFocusChanged({FocusInfo::WindowRestored, window});
}

void XEventHandler::windowFocusLost(Window &window) {
    onFocusChanged({FocusInfo::FocusLost, window});
}

void XEventHandler::windowFocus(Window &window) {
    onFocusChanged({FocusInfo::Focused, window});
}
#endif
}
