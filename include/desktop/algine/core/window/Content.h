#ifndef ALGINE_CONTENT_H
#define ALGINE_CONTENT_H

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

private:
    Window *m_window;
    uint m_width, m_height;
    bool m_isInitialized;
};
}

#endif //ALGINE_CONTENT_H
