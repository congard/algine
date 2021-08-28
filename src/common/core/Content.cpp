#include <algine/core/Content.h>

#include <algine/core/Surface.h>

namespace algine {
Content::Content()
    : m_surface(nullptr),
      m_isInitialized(false),
      m_width(),
      m_height() {}

Content::Content(Surface *surface)
    : m_isInitialized(false),
      m_width(),
      m_height()
{
    setSurface(surface);
}

Content::~Content() = default;

void Content::setSurface(Surface *surface) {
    surface->setContent(this);
}

Surface* Content::getSurface() const {
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
