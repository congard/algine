#include <algine/core/Content.h>

#include <algine/core/BaseWindow.h>

namespace algine {
Content::Content()
    : m_surface(nullptr),
      m_isInitialized(false),
      m_width(),
      m_height() {}

Content::Content(BaseWindow *surface)
    : m_isInitialized(false),
      m_width(),
      m_height()
{
    setSurface(surface);
}

Content::~Content() = default;

void Content::setSurface(BaseWindow *surface) {
    surface->setContent(this);
}

BaseWindow* Content::getSurface() const {
    return m_surface;
}

int Content::width() const {
    return m_width;
}

int Content::height() const {
    return m_height;
}

bool Content::isInitialized() const {
    return m_isInitialized;
}
}
