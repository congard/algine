#ifndef ALGINE_WIDGETS_LAYER_H
#define ALGINE_WIDGETS_LAYER_H

#include <algine/core/widgets/WidgetDisplayOptions.h>
#include <algine/core/math/Rect.h>
#include <algine/core/Object.h>

namespace algine {
class Container;
}

namespace algine::Widgets {
class Scene;

class AL_EXPORT Layer: public Object {
    friend class Scene;

public:
    explicit Layer(Scene *scene);
    ~Layer() override = default;

    virtual void draw(const WidgetDisplayOptions &options);

    void show(int level = -1);
    void hide();

    bool isVisible() const;

    void setParent(Object *parent) override;
    Scene* getParentWidgetsScene() const;

    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    float getOpacity() const;
    const RectI& getGeometry() const;

    void setContainer(Container *container);
    Container* getContainer() const;

protected:
    virtual void onGeometryChanged(const RectI &old, const RectI &current);
    virtual void onShow();
    virtual void onHide();

private:
    Container *m_container {nullptr};
};
}

#endif //ALGINE_WIDGETS_LAYER_H
