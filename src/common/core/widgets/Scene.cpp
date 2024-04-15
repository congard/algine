#include <algine/core/widgets/Scene.h>
#include <algine/core/widgets/Layer.h>
#include <algine/core/widgets/Container.h>

#include <tulz/Array.h>

using namespace tulz;

namespace algine::Widgets {
Scene::Scene(Object *parent)
    : algine::Scene(parent),
      m_painter(new Painter(this))
{
    m_options.painter = m_painter;
}

Scene::Scene(int width, int height, Object *parent)
    : Scene(parent)
{
    setSize(width, height);
}

/**
 * Layer will be added to the heap if and only if
 * it is opaque and isn't completely covered by others
 * @param heap sorted by rect's left coord
 * @param rect
 * @param insert must be set to false for non-opaque layers
 * @complexity <code>O(NlogN)</code>
 * @return <code>true</code> if <code>rect</code> is visible
 * (and was added to the heap, if specified), otherwise <code>false</code>
 */
static bool placeRect(std::vector<RectI> &heap, const RectI &rect, bool insert) {
    if (heap.empty()) {
        heap.emplace_back(rect);
        return true;
    }

    auto heap_cmp = [](const RectI &r1, const RectI &r2) {
        return r1.left() > r2.left();
    };

    auto insert_sorted = [&]() {
        if (insert) {
            heap.emplace_back(rect); // O(1) (memory has been already allocated)
            std::push_heap(heap.begin(), heap.end(), heap_cmp); // O(logN)
        }
    };

    int end = rect.left();
    auto workHeap = heap; // temporary heap, we will pop elements from here

    for (int i = 0; i < heap.size(); ++i) {
        std::pop_heap(workHeap.begin(), workHeap.end() - i, heap_cmp); // O(logN)
        RectI &r = workHeap[workHeap.size() - 1 - i];

        // ========+     +===============
        //        end   r.left()
        //          ^^^^^
        // Remember that rects are sorted by left x coord
        if (end < r.left()) {
            insert_sorted();
            return true;
        }

        // r.left()              r.right()
        //    +======================+
        // =============+
        //             end
        if (end < r.right() && r.top() <= rect.top() && rect.bottom() <= r.bottom()) {
            end = r.right();

            if (end >= rect.right()) {
                return false;
            }
        }
    }

    insert_sorted();

    return true;
}

void Scene::draw(int begin, int end) {
    if (m_layers.empty())
        return;

    if (end < 0)
        end = static_cast<int>(m_layers.size());

    std::vector<RectI> heap;
    heap.reserve(end - begin);

    if (auto layer = layerAt(end - 1); layer->getOpacity() == 1.0f)
        heap.emplace_back(layer->getGeometry());

    Array<bool> drawList(m_layers.size(), false);
    drawList[end - 1] = true;

    for (int i = end - 2; i >= begin; --i) {
        auto layer = layerAt(i);
        drawList[i] = placeRect(heap, layer->getGeometry(), layer->getOpacity() == 1.0f);
    }

    bool activatePainter = !m_painter->isActive();

    if (activatePainter)
        m_painter->begin();

    for (int i = begin; i < end; ++i) {
        if (drawList[i]) {
            layerAt(i)->draw(m_options);
        }
    }

    if (activatePainter) {
        m_painter->end();
    }
}

void Scene::showLayer(Layer *layer, int level) {
    assert(layer->getParent() == this);

    if (level == -1) {
        m_layers.emplace_back(layer);
    } else {
        m_layers.insert(m_layers.begin() + level, layer);
    }

    layer->getContainer()->requestLayout();
    layer->onShow();
}

void Scene::replaceLayer(Layer *src, Layer *replacement) {
    assert(src->getParent() == this);
    assert(replacement->getParent() == this);

    int level = getLevel(src);

    if (level == -1)
        throw std::runtime_error("Source Layer is not visible");

    m_layers[level] = replacement;

    src->onHide();

    replacement->getContainer()->requestLayout();
    replacement->onShow();
}

void Scene::hideLayer(Layer *layer) {
    int level = getLevel(layer);

    if (level == -1)
        throw std::runtime_error("Layer was not found at this Scene");

    hideLevelAt(level);
}

void Scene::hideLevelAt(int level) {
    m_layers[level]->onHide();
    m_layers.erase(m_layers.begin() + level);
}

Layer* Scene::layerAt(int level) const {
    return m_layers.at(level);
}

Layer* Scene::getTopLayer() const {
    return m_layers.back();
}

int Scene::getLevel(const Layer *layer) const {
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    return it == m_layers.end() ? -1 : static_cast<int>(std::distance(m_layers.begin(), it));
}

int Scene::getLayerCount() const {
    return static_cast<int>(m_layers.size());
}

void Scene::setFramebuffer(Framebuffer *framebuffer) {
    m_options.framebuffer = framebuffer;
}

Framebuffer* Scene::getFramebuffer() const {
    return m_options.framebuffer;
}

void Scene::setSize(int width, int height) {
    m_painter->setViewport(width, height);
    m_options.width = width;
    m_options.height = height;

    for (auto layer : m_layers) {
        layer->getContainer()->requestLayout();
    }
}

void Scene::setSize(const SizeI &size) {
    setSize(size.getWidth(), size.getHeight());
}

int Scene::getWidth() const {
    return m_options.width;
}

int Scene::getHeight() const {
    return m_options.height;
}

SizeI Scene::getSize() const {
    return {getWidth(), getHeight()};
}

bool Scene::event(const Event &event) {
    if (event.isPointer()) {
        return handlePointerEvent(event);
    } else if (event.isKeyboard()) {
        return handleKeyboardEvent(event);
    } else {
        return false;
    }
}

void Scene::listen(UnifiedEventHandler *eventHandler) {
    eventHandler->addPointerEventListener(getPointerEventListener());
    eventHandler->addKeyboardEventListener(getKeyboardEventListener());
    eventHandler->addWindowEventListener(getSurfaceEventListener());
}

UnifiedEventHandler::EventListener Scene::getPointerEventListener() {
    return [this](const Event &event) {
        this->event(event);
    };
}

UnifiedEventHandler::EventListener Scene::getKeyboardEventListener() {
    return getPointerEventListener();
}

UnifiedEventHandler::EventListener Scene::getSurfaceEventListener() {
    return [this](const Event &event) {
        switch (event.getId()) {
            case Event::WindowSizeChange:
                setSize(event.getSize());
                break;
            default: break;
        }
    };
}

Painter* Scene::getPainter() const {
    return m_painter;
}

void Scene::onRender() {
    draw();
}

bool Scene::handlePointerEvent(const Event &event) {
    auto &info = event.getPointerInfo();
    PointI globalPoint {(int) info.getX(), (int) info.getY()};

    // returns a widget on the scene at globalPoint
    auto findWidget = [this, &globalPoint]() -> Widget* {
        for (int i = getLayerCount() - 1; i >= 0; --i) {
            auto container = layerAt(i)->getContainer();
            PointI container_lp = container->mapFromGlobal(globalPoint);

            if (!container->getGeometry().contains(container_lp))
                continue;

            Widget* widget = container->notContainerAt(container_lp);

            if (!widget)
                widget = container->widgetAt(container_lp);

            if (!widget)
                widget = container;

            return widget;
        }

        return nullptr;
    };

    auto pushEvent = [&](Widget *widget) {
        PointI widget_lp = widget->mapFromGlobal(globalPoint);
        PointerInfo widgetInfo = info;
        widgetInfo.setPos({widget_lp.getX(), widget_lp.getY()});
        widget->event(Event(event.getId(), widgetInfo));
    };

    switch (event.getId()) {
        case Event::Press:
            if (auto widget = findWidget(); widget) {
                m_touchedWidgets[info.getPointer()] = widget;
                pushEvent(widget);
                return true;
            }
            return false;
        case Event::Click:
        case Event::Move:
            // click is called after release, so we should handle it separately;
            // move can be called even before press
            if (auto widget = findWidget(); widget) {
                pushEvent(widget);
                return true;
            }
            return false;
        case Event::Release:
            if (auto it = m_touchedWidgets.find(info.getPointer()); it != m_touchedWidgets.end()) {
                pushEvent(it->second);

                if (event.getId() == Event::Release)
                    m_touchedWidgets.erase(it);

                return true;
            }
            break;
        default: break;
    }

    return false;
}

bool Scene::handleKeyboardEvent(const Event &event) {
    return false; // TODO
}
}
