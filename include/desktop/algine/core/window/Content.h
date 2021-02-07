#ifndef ALGINE_CONTENT_H
#define ALGINE_CONTENT_H

#include <algine/core/input/KeyboardKey.h>
#include <algine/core/input/MouseKey.h>

#include <algine/types.h>

namespace algine {
class Window;

class Content {
    friend class Window;

public:
    Content();
    explicit Content(Window *window);
    virtual ~Content();

    virtual void init() = 0;

    virtual void render() = 0;

    void setWindow(Window *window);
    Window* getWindow() const;

    uint width() const;
    uint height() const;

    bool isInitialized() const;

    virtual void mouseMove(double x, double y);
    virtual void mouseClick(MouseKey key);
    virtual void mouseKeyPress(MouseKey key);
    virtual void mouseKeyRelease(MouseKey key);

    virtual void keyboardKeyPress(KeyboardKey key);
    virtual void keyboardKeyRelease(KeyboardKey key);
    virtual void keyboardKeyRepeat(KeyboardKey key);

    virtual void windowSizeChange(int width, int height);
    virtual void windowPosChange(int x, int y);

protected:
    bool isKeyPressed(KeyboardKey key) const;
    bool isMouseKeyPressed(MouseKey key) const;

private:
    Window *m_window;
    uint m_width, m_height;
    bool m_isInitialized;
};
}

#endif //ALGINE_CONTENT_H
