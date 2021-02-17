#include <algine/core/Screen.h>

#include <algine/core/Content.h>
#include <algine/core/ScreenEventHandler.h>

#include <stdexcept>

#include "Bridge.h"

namespace algine {
Screen* Screen::m_screen = nullptr;

Screen::Screen() {
    if (m_screen != nullptr) {
        throw std::runtime_error(
            "You cannot have more than 1 Screen instance. Use instance() to get the current"
        );
    }

    m_screen = this;

    initSurfaceFields();
}

void Screen::renderFrame() {
    m_content->render();
}

void Screen::renderLoop() {
    AndroidBridge::renderLoop();
}

void Screen::stopRenderLoop() {
    AndroidBridge::stopRenderLoop();
}

bool Screen::isRenderLoopRunning() const {
    return AndroidBridge::isRenderLoopRunning();
}

bool Screen::isActionHandlerAvailable() const {
    return m_eventHandler != nullptr && AndroidBridge::isActionHandlerAvailable();
}

bool Screen::isPointerAvailable(int pointerId) const {
    return AndroidBridge::isPointerAvailable(pointerId);
}

float Screen::getPointerX(int pointerId) const {
    return AndroidBridge::getPointerX(pointerId);
}

float Screen::getPointerY(int pointerId) const {
    return AndroidBridge::getPointerY(pointerId);
}

glm::vec2 Screen::getPointerPos(int pointerId) const {
    return AndroidBridge::getPointerPos(pointerId);
}

void Screen::destroyInstance() {
    deletePtr(m_screen)
}

void Screen::__resized(int width, int height) {
    if (m_screen) {
        m_screen->m_viewport.x = width;
        m_screen->m_viewport.y = height;

        if (auto &content = m_screen->m_content; content != nullptr) {
            content->m_width = width;
            content->m_height = height;
        }

        if (auto eventHandler = m_screen->m_eventHandler; eventHandler != nullptr) {
            reinterpret_cast<ScreenEventHandler *>(eventHandler)->surfaceResized(width, height);
        }
    }
}
}
