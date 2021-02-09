#include <algine/core/window/Content.h>

#include <algine/core/window/Window.h>

namespace algine {
Content::Content()
    : m_window(nullptr),
      m_isInitialized(false),
      m_width(),
      m_height() {}

Content::Content(Window *window)
    : m_isInitialized(false),
      m_width(),
      m_height()
{
    setWindow(window);
}

Content::~Content() = default;

void Content::setWindow(Window *window) {
    window->setContent(this);
}

Window* Content::getWindow() const {
    return m_window;
}

uint Content::width() const {
    return m_width;
}

uint Content::height() const {
    return m_height;
}

bool Content::isInitialized() const {
    return m_isInitialized;
}
}
