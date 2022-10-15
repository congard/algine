#ifndef ALGINE_WIDGETS_LAYER_H
#define ALGINE_WIDGETS_LAYER_H

#include <algine/core/widgets/WidgetDisplayOptions.h>
#include <algine/core/widgets/ContainerPtr.h>
#include <algine/core/math/Rect.h>

namespace algine::Widgets {
class Scene;

class Layer {
    friend class Scene;

public:
    Layer() = default;
    explicit Layer(Scene *scene);
    virtual ~Layer() = default;

    virtual void draw(const WidgetDisplayOptions &options);

    void show(int level = -1);
    void hide();

    bool isVisible() const;

    void setScene(Scene *scene);
    Scene* getScene() const;

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

    void setContainer(ContainerPtr container);
    const ContainerPtr& getContainer() const;

protected:
    virtual void onGeometryChanged(const RectI &old, const RectI &current);
    virtual void onShow();
    virtual void onHide();

private:
    ContainerPtr m_container;
    Scene *m_scene {nullptr};
};

using LayerPtr = std::shared_ptr<Layer>;
}

#endif //ALGINE_WIDGETS_LAYER_H
