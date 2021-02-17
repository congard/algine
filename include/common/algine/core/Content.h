#ifndef ALGINE_CONTENT_H
#define ALGINE_CONTENT_H

#include <algine/templates.h>
#include <algine/types.h>

namespace algine {
class Surface;

enable_if_desktop(class Window);
enable_if_android(class Screen);

class Content {
    friend class Surface;

    enable_if_desktop(friend class Window);
    enable_if_android(friend class Screen);

public:
    Content();
    explicit Content(Surface *surface);
    virtual ~Content();

    virtual void init() = 0;

    virtual void render() = 0;

    void setSurface(Surface *surface);
    Surface* getSurface() const;

    uint width() const;
    uint height() const;

    bool isInitialized() const;

    enable_if_desktop(
        inline auto getWindow() const {
            return reinterpret_cast<Window *>(m_surface);
        }
    )

    enable_if_android(
        inline auto getScreen() const {
            return reinterpret_cast<Screen *>(m_surface);
        }
    )

private:
    Surface *m_surface;
    uint m_width, m_height;
    bool m_isInitialized;
};
}

#endif //ALGINE_CONTENT_H
