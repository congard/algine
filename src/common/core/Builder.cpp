#include <algine/core/Builder.h>
#include <algine/core/scene/GlobalScene.h>

using namespace std;

namespace algine {
Builder::Builder()
    : m_parent(GlobalScene::getInstance()),
      m_scope(Scope::ParentObject) {}

void Builder::setName(const string &name) {
    m_name = name;
}

const string& Builder::getName() const {
    return m_name;
}

void Builder::setParent(Object *parent) {
    m_parent = parent;
}

Object* Builder::getParent() {
    return m_parent;
}

void Builder::setScope(Scope scope) {
    m_scope = scope;
}

Builder::Scope Builder::getScope() const {
    return m_scope;
}

Object* Builder::getActualParent() {
    if (m_parent == nullptr)
        return nullptr;
    if (m_scope == Scope::ParentObject)
        return m_parent;
    return m_parent->getParentScene();
}
}
