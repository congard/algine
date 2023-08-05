#include <algine/core/widgets/Layer.h>
#include <algine/core/widgets/Scene.h>
#include <algine/core/widgets/Container.h>

namespace algine::Widgets {
Layer::Layer(Scene *scene)
    : Object(scene) {}

void Layer::draw(const WidgetDisplayOptions &options) {
    m_container->display(options);
}

void Layer::show(int level) {
    getParentWidgetsScene()->showLayer(this, level);
}

void Layer::hide() {
    getParentWidgetsScene()->hideLayer(this);
}

bool Layer::isVisible() const {
    return getParentWidgetsScene()->getLevel(this) != -1;
}

void Layer::setParent(Object *parent) {
    if (dynamic_cast<Widgets::Scene*>(parent) == nullptr)
        throw std::runtime_error("Invalid parent type: only Widgets::Scene is acceptable");
    Object::setParent(parent);
}

Scene* Layer::getParentWidgetsScene() const {
    return dynamic_cast<Widgets::Scene*>(getParent());
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

void Layer::setContainer(Container *container) {
    m_container = container;
    m_container->setParent(this);
}

Container* Layer::getContainer() const {
    return m_container;
}

void Layer::onGeometryChanged(const RectI &old, const RectI &current) {}

void Layer::onShow() {}

void Layer::onHide() {}
}
