#include <algine/core/x/XEventHandler.h>
#include <algine/core/x/Event.h>
#include <algine/core/math/Size.h>

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
}
