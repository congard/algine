#include <algine/core/Content.h>

#include <algine/core/Window.h>

namespace algine {
Content::Content(Object *parent)
    : Object(parent),
      m_width(),
      m_height() {}

Content::~Content() = default;

void Content::onShow() {}

void Content::onHide() {}

void Content::setWindow(Window *window) {
    window->setContent(this);
}

Window* Content::getWindow() const {
    return dynamic_cast<Window*>(getParent());
}

int Content::width() const {
    return m_width;
}

int Content::height() const {
    return m_height;
}
}
