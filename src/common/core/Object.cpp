#include <algine/core/Object.h>
#include <algine/core/scene/GlobalScene.h>
#include <algine/core/widgets/Widget.h>
#include <algine/core/Window.h>

#include <cxxabi.h>

namespace algine {
Object::Object(Object *parent) {
    if (parent != nullptr) {
        parent->addChild(this);
    }

#ifdef ALGINE_SECURE_OPERATIONS
    ++_m_sopAllocSummary.alloc;
#endif
}

Object::~Object() {
    // Make a copy, because the original list
    // will be changing during the destruction
    auto children = getChildren();

    for (auto child : children) {
        delete child;
    }

    if (getParent() != nullptr) {
        getParent()->removeChild(this, FindOption::Direct);
    }

#ifdef ALGINE_SECURE_OPERATIONS
    ++_m_sopAllocSummary.dealloc;
#endif
}

void Object::setName(const std::string &name) {
    m_name = name;
}

const std::string& Object::getName() const {
    return m_name;
}

const Object::ObjectList& Object::getChildren() const {
    return m_children;
}

void Object::setParent(Object *parent) {
    if (parent == nullptr) {
        // remove parent
        if (getParent() != nullptr) {
            getParent()->removeChild(this, FindOption::Direct);
        }
    } else {
        parent->addChild(this);
    }
}

Object* Object::getParent() const {
    return m_parent;
}

Scene* Object::getParentScene() const {
    return findParent<Scene*>();
}

Window* Object::getParentWindow() const {
    return findParent<Window*>();
}

bool Object::isScene() const {
    return is<const Scene*>();
}

bool Object::isWidget() const {
    return is<const Widget*>();
}

void Object::setThreadSafety(bool enable) {
    if (enable) {
        if (m_mutex == nullptr) {
            m_mutex = std::make_unique<std::mutex>();
        }
    } else {
        m_mutex.reset();
    }
}

bool Object::isThreadSafety() const {
    return m_mutex != nullptr;
}

bool Object::hasChild(Object *obj, FindOption option) {
    Locker locker(m_mutex);

    auto &children = getChildren();

    // step 1: find direct children
    for (auto child : children) {
        if (obj == child) {
            return true;
        }
    }

    // step 2: find recursively if specified
    for (auto child : children) {
        if (child->hasChild(obj, option)) {
            return true;
        }
    }

    return false;
}

Object* Object::getParentSceneAsObject() const {
    return getParentScene();
}

void Object::addChild(Object *child) {
    if (this == child->getParent())
        return;

    Locker locker(m_mutex);

    // remove previous parent
    if (child->getParent() != nullptr)
        child->getParent()->removeChild(child, FindOption::Direct);

    // set current parent & add to list
    m_children.emplace_front(child);
    child->m_parent = this;
}

bool Object::removeChild(Object *child, FindOption option) {
    Locker locker(m_mutex);

    // step 1: find direct children
    auto it_prev = m_children.before_begin();
    auto it = m_children.begin();

    while (it != m_children.end()) {
        if (*it == child) {
            m_children.erase_after(it_prev);
            child->m_parent = nullptr;
            onChildRemoved(child);
            return true;
        }

        ++it;
        ++it_prev;
    }

    // step 2: find recursively if specified
    if (option == FindOption::FindRecursively) {
        for (auto m_child : m_children) {
            if (m_child->removeChild(child, option)) {
                return true;
            }
        }
    }

    // step 3: Find direct children of the
    // current object and its scene if specified
    if (option == FindOption::DirectThisAndScene) {
        if (auto scene = getParentSceneAsObject(); scene != nullptr) {
            if (scene->removeChild(child, FindOption::Direct)) {
                return true;
            }
        }
    }

    return false;
}

void Object::onChildRemoved(Object *child) {}

Object* Object::defaultParent() {
    return GlobalScene::getInstance();
}

std::string Object::getTypeName(std::string_view mangledName) {
    const auto dmg = abi::__cxa_demangle(mangledName.data(), nullptr, nullptr, nullptr);
    std::string name = dmg;
    std::free(dmg);
    return name;
}

Object::Locker::Locker(std::unique_ptr<std::mutex> &mutex)
    : m_mutex(mutex)
{
    if (m_mutex != nullptr) {
        m_mutex->lock();
    }
}

Object::Locker::~Locker() {
    if (m_mutex != nullptr) {
        m_mutex->unlock();
    }
}
}
