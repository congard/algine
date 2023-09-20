#ifndef ALGINE_WIDGETS_SCENE_H
#define ALGINE_WIDGETS_SCENE_H

#include <algine/core/widgets/WidgetDisplayOptions.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/math/Size.h>
#include <algine/core/unified/UnifiedEventHandler.h>

namespace algine {
class Widget;
}

namespace algine::Widgets {
class Layer;

/**
 * A special class that makes it easy to create UI.
 * Parent class for UI Layers.
 * <br>Usage:
 * <ol>
 *   <li>Inherit your custom scene from <code>algine::Widgets::Scene</code></li>
 *   <li>Load layers (make them as children of the scene)</li>
 *   <li>Add listeners to <code>XEventHandler</code>: call <code>listen(handler)</code>
 *   <li>Call <code>setSize(width, height)</code></li>
 *   <li>Show your start layer: <code>scene->showLayer(scene->myStartLayer)</code></li>
 *   <li>In the render loop draw UI: <code>scene.draw()</code></li>
 * </ol>
 */
class AL_EXPORT Scene: public Object {
public:
    explicit Scene(Object *parent = defaultParent());
    Scene(int width, int height, Object *parent = defaultParent());
    ~Scene() override = default;

    /**
     * Draws layers from level <code>begin</code> to <code>end</code>
     * <br>If <code>end</code> is less than zero, it will be set to
     * layers count
     * @param begin
     * @param end
     */
    void draw(int begin = 0, int end = -1);

    void showLayer(Layer *layer, int level = -1);
    void showLayerInsteadOf(Layer *replacement, Layer *src);
    void hideLayer(Layer *layer);
    void hideLevel(int level);

    Layer* layerAt(int level) const;
    Layer* getTopLayer() const;

    int getLevel(const Layer *layer) const;
    int getLayersCount() const;

    /**
     * @note ownership of the object will not be transferred to the scene
     * @param framebuffer The framebuffer to draw UI into
     */
    void setFramebuffer(Framebuffer *framebuffer);

    Framebuffer* getFramebuffer() const;

    virtual void setSize(int width, int height);
    void setSize(const SizeI &size);

    int getWidth() const;
    int getHeight() const;

    SizeI getSize() const;

    bool event(const Event &event);

    /**
     * Adds listeners to <code>eventHandler</code>
     * @param eventHandler
     */
    void listen(UnifiedEventHandler *eventHandler);

    virtual UnifiedEventHandler::EventListener getPointerEventListener();
    virtual UnifiedEventHandler::EventListener getKeyboardEventListener();
    virtual UnifiedEventHandler::EventListener getSurfaceEventListener();

    Painter* getPainter() const;

private:
    bool handlePointerEvent(const Event &event);
    bool handleKeyboardEvent(const Event &event);

private:
    std::vector<Layer*> m_layers;
    Painter *m_painter;
    WidgetDisplayOptions m_options;
    std::unordered_map<PointerInfo::Pointer, Widget*> m_touchedWidgets;
};
}

#endif //ALGINE_WIDGETS_SCENE_H
