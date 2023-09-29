#ifndef ALGINE_SCENE_H
#define ALGINE_SCENE_H

#include <algine/core/Object.h>

namespace algine {
class AL_EXPORT Scene: public Object {
public:
    explicit Scene(Object *parent = nullptr);

    /**
     * Shows the current scene, if it is hidden.
     * @note `onShow` will be called only if the
     * current scene is hidden.
     */
    void show();

    /**
     * Hides the current scene, if it is shown.
     * @note `onHide` will be called only if the
     * current scene is shown.
     */
    void hide();

    /**
     * Renders the current scene if it is shown.
     * @note `onRender` will be called only if the
     * current scene is shown.
     */
    void render();

    bool isShown() const;
    bool isHidden() const;

    using Object::addChild;
    using Object::removeChild;
    using Object::removeChildren;

protected:
    virtual void onShow();
    virtual void onHide();
    virtual void onRender();

private:
    bool m_isShown;
};
} // algine

#endif //ALGINE_SCENE_H
