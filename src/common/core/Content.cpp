#include <algine/core/Content.h>

#include <algine/core/BaseWindow.h>

namespace algine {
Content::Content(Object *parent)
    : Object(parent),
      m_width(),
      m_height() {}

Content::~Content() = default;

void Content::onShow() {}

void Content::onHide() {}

void Content::setWindow(BaseWindow *window) {
    window->setContent(this);
}

BaseWindow* Content::getWindow() const {
    return dynamic_cast<BaseWindow*>(getParent());
}

int Content::width() const {
    return m_width;
}

int Content::height() const {
    return m_height;
}
}
