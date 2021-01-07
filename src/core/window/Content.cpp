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

void Content::mouseMove(double x, double y) {}
void Content::mouseClick(MouseKey key) {}
void Content::mouseKeyPress(MouseKey key) {}
void Content::mouseKeyRelease(MouseKey key) {}

void Content::keyboardKeyPress(KeyboardKey key) {}
void Content::keyboardKeyRelease(KeyboardKey key) {}
void Content::keyboardKeyRepeat(KeyboardKey key) {}

void Content::windowSizeChange(int width, int height) {}
void Content::windowPosChange(int x, int y) {}

bool Content::isKeyPressed(KeyboardKey key) const {
    return m_window->isKeyPressed(key);
}

bool Content::isMouseKeyPressed(MouseKey key) const {
    return m_window->isMouseKeyPressed(key);
}
}
