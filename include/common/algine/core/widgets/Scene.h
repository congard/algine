#ifndef ALGINE_SCENE_H
#define ALGINE_SCENE_H

#include <algine/core/widgets/WidgetDisplayOptions.h>
#include <algine/core/widgets/WidgetPtr.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/PtrView.h>
#include <algine/core/XEventHandler.h>

namespace algine::Widgets {
class Layer;

/**
 * A special class that makes it easy to create UI
 * <br>Usage:
 * <ol>
 *   <li>Inherit your custom scene from <code>algine::Widgets::Scene</code></li>
 *   <li>Load layers (store them for example as smart pointers)</li>
 *   <li>Add listeners to <code>XEventHandler</code>: call <code>listen(handler)</code>
 *   <li>Call <code>setSize(width, height)</code></li>
 *   <li>Show your start layer: <code>scene.showLayer(scene.myStartLayer)</code></li>
 *   <li>In the render loop draw UI: <code>scene.draw()</code></li>
 * </ol>
 */
class Scene {
public:
    Scene();
    Scene(int width, int height);
    virtual ~Scene() = default;

    /**
     * Draws layers from level <code>begin</code> to <code>end</code>
     * <br>If <code>end</code> is less than zero, it will be set to
     * layers count
     * @param begin
     * @param end
     */
    void draw(int begin = 0, int end = -1);

    void showLayer(PtrView<Layer> layer, int level = -1);
    void showLayerInsteadOf(PtrView<Layer> replacement, PtrView<Layer> src);
    void hideLayer(PtrView<Layer> layer);
    void hideLevel(int level);

    Layer* layerAt(int level) const;
    Layer* getTopLayer() const;

    int getLevel(PtrView<const Layer> layer) const;
    int getLayersCount() const;

    void setFramebuffer(PtrView<Framebuffer> framebuffer);
    void setSize(int width, int height);

    bool event(const Event &event);

    /**
     * Adds listeners to <code>eventHandler</code>
     * @param eventHandler
     */
    void listen(XEventHandler *eventHandler);

    virtual XEventHandler::EventListener getPointerEventListener();
    virtual XEventHandler::EventListener getKeyboardEventListener();
    virtual XEventHandler::EventListener getSurfaceEventListener();

    const std::unique_ptr<Painter>& getPainter() const;

private:
    bool handlePointerEvent(const Event &event);
    bool handleKeyboardEvent(const Event &event);

    void applySizePolicy(Layer *layer) const;

private:
    std::vector<Layer*> m_layers;
    std::unique_ptr<Painter> m_painter;
    WidgetDisplayOptions m_options;
    std::unordered_map<XEventHandler::PointerInfo::Pointer, WidgetPtr> m_touchedWidgets;
};
}

#endif //ALGINE_SCENE_H
