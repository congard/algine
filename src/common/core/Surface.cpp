#include <algine/core/Surface.h>

#include <algine/core/Content.h>

namespace algine {
Surface::~Surface() = default;

glm::ivec2 Surface::getViewport() {
    return m_viewport;
}

void Surface::setContent(const Ptr<Content> &content) {
    if (m_content)
        m_content->m_surface = nullptr;

    m_content = content;

    if (!m_content) {
        stopRenderLoop();
    } else {
        glm::ivec2 viewport = getViewport();

        m_content->m_surface = this;
        m_content->m_width = viewport.x;
        m_content->m_height = viewport.y;

        if (!m_content->isInitialized()) {
            m_content->init();
            m_content->m_isInitialized = true;
        }
    }
}

void Surface::setContent(Content *content) {
    setContent(Ptr<Content>(content));
}

const Ptr<Content>& Surface::getContent() const {
    return m_content;
}

void Surface::setEventHandler(EventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

EventHandler* Surface::getEventHandler() const {
    return m_eventHandler;
}

void Surface::initSurfaceFields() {
    m_content = nullptr;
    m_eventHandler = nullptr;
    m_viewport = {0, 0};
}
}
