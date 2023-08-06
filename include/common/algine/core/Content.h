#ifndef ALGINE_CONTENT_H
#define ALGINE_CONTENT_H

#include <algine/core/Object.h>
#include <algine/templates.h>
#include <algine/types.h>

namespace algine {
class BaseWindow;

#ifdef ALGINE_QT_PLATFORM
    #define PlatformFriend QtWindow
#elif defined(__ANDROID__)
    #define PlatformFriend Screen
#else
    #define PlatformFriend Window
#endif

class PlatformFriend;

class Content: public Object {
    friend class BaseWindow;
    friend class PlatformFriend;

public:
    /**
     * @note constructor must be called from the rendering thread.
     * @see `BaseWindow::setContentLater`
     */
    explicit Content(Object *parent = defaultParent());

    ~Content() override;

    virtual void render() = 0;

    virtual void onShow();
    virtual void onHide();

    void setWindow(BaseWindow *window);
    BaseWindow* getWindow() const;

    int width() const;
    int height() const;

private:
    int m_width;
    int m_height;
};
}

#endif //ALGINE_CONTENT_H
