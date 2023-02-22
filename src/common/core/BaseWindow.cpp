#include <algine/core/BaseWindow.h>

#include <algine/core/Content.h>

namespace algine {
BaseWindow::~BaseWindow() = default;

void BaseWindow::setContent(const Ptr<Content> &content) {
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

void BaseWindow::setContent(Content *content) {
    setContent(Ptr<Content>(content));
}

const Ptr<Content>& BaseWindow::getContent() const {
    return m_content;
}

void BaseWindow::setEventHandler(EventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

EventHandler* BaseWindow::getEventHandler() const {
    return m_eventHandler;
}
}
