#include <algine/core/BaseWindow.h>

#include <algine/core/Content.h>

namespace algine {
BaseWindow::BaseWindow(Object *parent)
    : Object(parent),
      m_taskQueue(),
      m_taskQueueTail(m_taskQueue.before_begin()) {}

BaseWindow::~BaseWindow() = default;

void BaseWindow::invokeLater(Task task) {
    std::lock_guard locker(m_taskQueueMutex);
    m_taskQueueTail = m_taskQueue.emplace_after(m_taskQueueTail, std::move(task));
}

void BaseWindow::setContent(Content *content, bool deleteOld) {
    if (m_content) {
        m_content->onHide();

        if (deleteOld) {
            delete m_content;
        }
    }

    m_content = content;

    if (m_content == nullptr)
        return;

    m_content->setParent(this);

    glm::ivec2 viewport = getViewport();
    m_content->m_width = viewport.x;
    m_content->m_height = viewport.y;

    m_content->onShow();
}

void BaseWindow::setContent(Content *content) {
    setContent(content, true);
}

Content* BaseWindow::getContent() const {
    return m_content;
}

void BaseWindow::setEventHandler(EventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

EventHandler* BaseWindow::getEventHandler() const {
    return m_eventHandler;
}

void BaseWindow::processTasks() {
    std::lock_guard locker(m_taskQueueMutex);

    if (m_taskQueue.empty())
        return;

    for (Task &task : m_taskQueue)
        task();

    m_taskQueue.clear();
    m_taskQueueTail = m_taskQueue.before_begin();
}
}
