#include <algine/core/Window.h>

#include <algine/core/Content.h>

namespace algine {
Window::Window(Object *parent)
    : Object(parent),
      m_taskQueue(),
      m_taskQueueTail(m_taskQueue.before_begin()) {}

Window::~Window() = default;

void Window::invokeLater(Task task) {
    std::lock_guard locker(m_taskQueueMutex);
    m_taskQueueTail = m_taskQueue.emplace_after(m_taskQueueTail, std::move(task));
}

void Window::setContent(Content *content, bool deleteOld) {
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

void Window::setContent(Content *content) {
    setContent(content, true);
}

void Window::setContentLater(Content *content, bool deleteOld) {
    invokeLater([=] {
        setContent(content, deleteOld);
    });
}

Content* Window::getContent() const {
    return m_content;
}

void Window::setEventHandler(EventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

EventHandler* Window::getEventHandler() const {
    return m_eventHandler;
}

void Window::processTasks() {
    std::lock_guard locker(m_taskQueueMutex);

    if (m_taskQueue.empty())
        return;

    for (Task &task : m_taskQueue)
        task();

    m_taskQueue.clear();
    m_taskQueueTail = m_taskQueue.before_begin();
}
}
