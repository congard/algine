#include <algine/core/widgets/Layer.h>
#include <algine/core/widgets/Scene.h>
#include <algine/core/widgets/Container.h>
#include <algine/core/XEventHandler.h>

namespace algine::Widgets {
Layer::Layer(Scene *scene) {
    setScene(scene);
}

void Layer::draw(const WidgetDisplayOptions &options) {
    m_container->display(options);
}

void Layer::show(int level) {
    assert(m_scene != nullptr);
    m_scene->showLayer(this, level);
}

void Layer::hide() {
    assert(m_scene != nullptr);
    m_scene->hideLayer(this);
}

bool Layer::isVisible() const {
    assert(m_scene != nullptr);
    return m_scene->getLevel(this) != -1;
}

void Layer::setScene(Scene *scene) {
    m_scene = scene;
}

Scene* Layer::getScene() const {
    return m_scene;
}

#define geometryCallback(...) \
RectI old = getGeometry(); \
__VA_ARGS__; \
if (const RectI &current = getGeometry(); current != old) { \
    onGeometryChanged(old, current); \
}

void Layer::setX(int x) {
    geometryCallback(m_container->setX(x))
}

void Layer::setY(int y) {
    geometryCallback(m_container->setY(y))
}

void Layer::setWidth(int width) {
    geometryCallback(m_container->setWidth(width))
}

void Layer::setHeight(int height) {
    geometryCallback(m_container->setHeight(height))
}

int Layer::getX() const {
    return m_container->getX();
}

int Layer::getY() const {
    return m_container->getY();
}

int Layer::getWidth() const {
    return m_container->getWidth();
}

int Layer::getHeight() const {
    return m_container->getHeight();
}

float Layer::getOpacity() const {
    return m_container->getOpacity();
}

const RectI& Layer::getGeometry() const {
    return m_container->getGeometry();
}

void Layer::setContainer(ContainerPtr container) {
    m_container = std::move(container);
    m_container->setParent(this);
}

const ContainerPtr& Layer::getContainer() const {
    return m_container;
}

void Layer::onGeometryChanged(const RectI &old, const RectI &current) {}

void Layer::onShow() {}

void Layer::onHide() {}
}
