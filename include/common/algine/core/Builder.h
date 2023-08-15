#ifndef ALGINE_BUILDER_H
#define ALGINE_BUILDER_H

#include <algine/core/lua/Scriptable.h>
#include <algine/core/Object.h>

#include <string>

namespace algine {
class Builder: public virtual Scriptable {
public:
    enum class Scope {
        // The object will be created in the parent scope. Before
        // creation, it will be checked whether an object with the
        // same name already exists in the parent scope
        ParentObject,

        // The object will be created in the scope of parent's
        // scene. If it doesn't exist, the object will be created
        // in the parent's scope. Before creation, it will be
        // checked whether an object with the same name already
        // exists in the scope of the parent's scene (or in the
        // parent's scope, if the parent doesn't have a scene)
        ParentScene
    };

public:
    Builder();

    void setName(const std::string &name);
    const std::string& getName() const;

    void setParent(Object *parent);
    Object* getParent();

    void setScope(Scope scope);
    Scope getScope() const;

protected:
    /**
     * Searches for object of type <code>T</code> named <code>name</code>.
     * @tparam T
     * @param name
     * @return the object of type `T` named `name`. If actual parent is `null`,
     * returns `null`.
     */
    template<typename T>
    T getByName(std::string_view name) {
        if (auto parent = getActualParent(); parent == nullptr) {
            return nullptr;
        } else {
            return parent->findChild<T>(name, Object::FindOption::Direct);
        }
    }

    template<typename T>
    T getByName() {
        return getByName<T>(m_name);
    }

    /**
     * Should be used in <pre>get()</pre>
     * @tparam T
     * @return
     */
    template<typename T>
    T getImpl() {
        if (m_name.empty())
            return createNew<T>();
        else if (auto obj = getByName<T>(); obj != nullptr)
            return obj;
        return createNew<T>();
    }

    /**
     * Should be used in <pre>create()</pre>
     * @tparam T
     * @return
     */
    template<typename T>
    T createNew() {
        if (!m_name.empty() && getByName<T>() != nullptr) {
            throw std::runtime_error(
                Object::getTypeName<std::remove_pointer_t<T>>() +
                " with the same name ('" + m_name + "') has been already loaded");
        }

        auto obj = dynamic_cast<T>(createImpl());
        assert(obj != nullptr);

        return obj;
    }

    /**
     * Returns the actual parent based on the Scope value.
     * @return The actual parent
     */
    Object* getActualParent();

    virtual Object* createImpl() = 0;

protected:
    std::string m_name;
    Scope m_scope;
    Object *m_parent;
};
}

#define AL_BUILDER(__object_name) \
public: \
    using result_t = __object_name; \
    __object_name* get() { return getImpl<__object_name*>(); } \
    __object_name* create() { return createNew<__object_name*>(); }

#endif //ALGINE_BUILDER_H
