#include <algine/core/scene/GlobalScene.h>
#include <algine/core/Engine.h>

namespace algine {
STATIC_INITIALIZER_IMPL(GlobalScene) {
    Engine::addOnDestroyListener([]() {
        delete m_instance;
    });
}

GlobalScene *GlobalScene::m_instance {nullptr};

// GlobalScene cannot have a parent
GlobalScene::GlobalScene()
    : Scene(nullptr) {}

GlobalScene* GlobalScene::getInstance() {
    if (m_instance == nullptr)
        m_instance = new GlobalScene();
    return m_instance;
}

void GlobalScene::setParent(Object *parent) {
    throw std::runtime_error("GlobalScene: unsupported operation setParent");
}
}
