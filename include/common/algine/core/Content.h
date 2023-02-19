#ifndef ALGINE_CONTENT_H
#define ALGINE_CONTENT_H

#include <algine/templates.h>
#include <algine/types.h>

namespace algine {
class Surface;

#ifdef ALGINE_QT_PLATFORM
    #define PlatformFriend QtWindow
#elif defined(__ANDROID__)
    #define PlatformFriend Screen
#else
    #define PlatformFriend Window
#endif

class PlatformFriend;

class Content {
    friend class Surface;
    friend class PlatformFriend;

public:
    Content();
    explicit Content(Surface *surface);
    virtual ~Content();

    virtual void init() = 0;

    virtual void render() = 0;

    void setSurface(Surface *surface);
    Surface* getSurface() const;

    int width() const;
    int height() const;

    bool isInitialized() const;

    enable_if_desktop(
        inline auto getWindow() const {
            return reinterpret_cast<PlatformFriend*>(m_surface);
        }
    )

    enable_if_android(
        inline auto getScreen() const {
            return reinterpret_cast<Screen*>(m_surface);
        }
    )

private:
    Surface *m_surface;
    int m_width, m_height;
    bool m_isInitialized;
};
}

#endif //ALGINE_CONTENT_H
