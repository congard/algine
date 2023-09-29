#include <algine/core/scene/Scene.h>

namespace algine {
Scene::Scene(Object *parent)
    : Object(parent),
      m_isShown(false) {}

void Scene::show() {
    if (isHidden()) {
        onShow();
        m_isShown = true;
    }
}

void Scene::hide() {
    if (isShown()) {
        onHide();
        m_isShown = false;
    }
}

void Scene::render() {
    if (isShown()) {
        onRender();
    }
}

bool Scene::isShown() const {
    return m_isShown;
}

bool Scene::isHidden() const {
    return !m_isShown;
}

void Scene::onShow() {}
void Scene::onHide() {}
void Scene::onRender() {}
} // algine