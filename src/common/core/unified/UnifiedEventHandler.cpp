#include <algine/core/unified/UnifiedEventHandler.h>
#include <algine/core/unified/Event.h>
#include <algine/core/math/Size.h>

namespace algine {
template<typename T>
inline void pushEvent(Event::Id id, const T &data,
                      std::forward_list<UnifiedEventHandler::EventListener> &listeners)
{
    Event event(id, data);

    for (auto &listener : listeners) {
        listener(event);
    }
}

void UnifiedEventHandler::onPointerMove(PointerInfo info) {
    pushEvent(Event::Move, info, m_pointerListeners);
}

void UnifiedEventHandler::onPointerClick(PointerInfo info) {
    pushEvent(Event::Click, info, m_pointerListeners);
}

void UnifiedEventHandler::onPointerPress(PointerInfo info) {
    pushEvent(Event::Press, info, m_pointerListeners);
}

void UnifiedEventHandler::onPointerRelease(PointerInfo info) {
    pushEvent(Event::Release, info, m_pointerListeners);
}

void UnifiedEventHandler::onKeyboardKeyPress(KeyboardKey key) {
    pushEvent(Event::KeyPress, key, m_keyboardListeners);
}

void UnifiedEventHandler::onKeyboardKeyRelease(KeyboardKey key) {
    pushEvent(Event::KeyRelease, key, m_keyboardListeners);
}

void UnifiedEventHandler::onKeyboardKeyRepeat(KeyboardKey key) {
    pushEvent(Event::KeyRepeat, key, m_keyboardListeners);
}

void UnifiedEventHandler::onSizeChanged(int width, int height) {
    pushEvent(Event::SurfaceSizeChange, SizeI(width, height), m_surfaceListeners);
}

void UnifiedEventHandler::onFocusChanged(FocusInfo info) {
    pushEvent(Event::SurfaceFocusChange, info, m_surfaceListeners);
}

void UnifiedEventHandler::addPointerEventListener(const EventListener &listener) {
    m_pointerListeners.emplace_front(listener);
}

void UnifiedEventHandler::addKeyboardEventListener(const EventListener &listener) {
    m_keyboardListeners.emplace_front(listener);
}

void UnifiedEventHandler::addSurfaceEventListener(const EventListener &listener) {
    m_surfaceListeners.emplace_front(listener);
}

void UnifiedEventHandler::clearPointerEventListeners() {
    m_pointerListeners.clear();
}

void UnifiedEventHandler::clearKeyboardEventListeners() {
    m_keyboardListeners.clear();
}

void UnifiedEventHandler::clearSurfaceEventListeners() {
    m_surfaceListeners.clear();
}
}
