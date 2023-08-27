#ifndef ALGINE_OBJECT_H
#define ALGINE_OBJECT_H

#include <algine/core/lua/Scriptable.h>

#include <string>
#include <forward_list>
#include <memory>
#include <atomic>

namespace algine {
class Scene;
class Window;

class Object {
    using ObjectList = std::forward_list<Object*>;

public:
    enum class FindOption {
        // Find direct children only
        Direct,

        // Find children recursively
        FindRecursively,

        // Find direct children of the current object
        // and direct children of its scene, in case
        // if it exists. If the current object is a
        // scene itself, then this flag is the same as
        // <code>FindOption::Direct</code>
        DirectThisAndScene
    };

    enum IterationFlag {
        // Stops iterating
        Break = 0b001,

        // Continues iterating
        Continue = 0b010,

        // Recursively iterates over children
        Children = 0b100,
    };

public:
    explicit Object(Object *parent = defaultParent());
    virtual ~Object();

    /// #solgen #ignore
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    void setName(const std::string &name);
    const std::string& getName() const;

    const ObjectList& getChildren() const;

    virtual void setParent(Object *parent);
    Object* getParent() const;

    /**
     * Returns the parent scene of this object.
     * @note This function may be expensive, since it will iterate
     * over parents until the scene is found (or nullptr is reached)
     * @return A pointer to the parent Scene object.
     */
    Scene* getParentScene() const;

    /**
     * Returns the parent window of this object.
     * @note This function may be expensive, since it will iterate
     * over parents until the window is found (or nullptr is reached)
     * @return A pointer to the parent Window object.
     */
    Window* getParentWindow() const;

    bool isScene() const;
    bool isWidget() const;

    /**
     * If enabled, additional synchronization will be performed
     * for functions like `hasChild`, `forEachChild`, `findChild` etc.
     * Enable it if you want to use this object from different
     * threads (add / remove children).
     * @param enable
     */
    void setThreadSafety(bool enable);

    /**
     * @return `true` if synchronization is enabled, `false` otherwise
     */
    bool isThreadSafety() const;

    template<typename T>
    bool is() const {
        return dynamic_cast<std::add_const_t<std::remove_pointer_t<T>>*>(this);
    }

    template<typename T>
    T as() {
        return dynamic_cast<T>(this);
    }

    /**
     * Returns the parent of type `T`.
     * @note This function may be expensive, since it will iterate
     * over parents until the parent of type `T` is found (or nullptr
     * is reached).
     * @tparam T The type of the parent.
     * @return A pointer to the parent object of type `T`.
     */
    template<typename T>
    T findParent() const {
        if (auto parent = m_parent->as<T>(); parent != nullptr)
            return parent;
        else if (m_parent != nullptr)
            return m_parent->findParent<T>();
        return nullptr;
    }

    bool hasChild(Object *obj, FindOption option = FindOption::FindRecursively);

    /**
     * Iterates over children.
     * @tparam T The type of the children (pointer)
     * @tparam C The type of the consumer callable
     * @param consumer Consumer that accepts T as an argument and returns
     * a combination of <code>Object::IterationFlag</code>:
     * <ul>
     *   <li>if <code>Object::IterationFlag::Children</code> is enabled, then
     *       <code>forEachChild</code> will also be called for the current child;</li>
     *   <li>if <code>Object::IterationFlag::Continue</code> is enabled, then
     *       iteration continues;</li>
     *   <li>otherwise, if <code>Object::IterationFlag::Break</code> is enabled, then
     *       iteration stops.</li>
     * </ul>
     */
    template<typename T = Object*, typename C>
    void forEachChild(C &&consumer) {
        static_assert(std::is_invocable_r_v<int, C, T>);

        Locker locker(m_mutex);
        auto &children = getChildren();

        for (auto child : children) {
            if (auto tChild = child->as<T>(); tChild != nullptr) {
                auto flags = consumer(tChild);

                if (flags & IterationFlag::Children) {
                    forEachChild<T>(consumer);
                }

                if (flags & IterationFlag::Break) {
                    break;
                }
            }
        }
    }

    template<typename T>
    T findChild(FindOption option = FindOption::FindRecursively) {
        return findChild<T>([](T) { return true; }, option);
    }

    template<typename T>
    T findChild(std::string_view name, FindOption option = FindOption::FindRecursively) {
        return findChild<T>([name](T child) { return child->getName() == name; }, option);
    }

    template<typename T, typename P>
    std::enable_if_t<std::is_invocable_r_v<bool, P, T>, T>
    findChild(P &&pred, FindOption option = FindOption::FindRecursively) {
        Locker locker(m_mutex);

        auto &children = getChildren();

        // step 1: find direct children
        for (auto child : children) {
            if (auto tChild = child->as<T>(); tChild != nullptr && pred(tChild)) {
                return tChild;
            }
        }

        // step 2: find recursively if specified
        if (option == FindOption::FindRecursively) {
            for (auto child : children) {
                if (auto tChild = child->findChild<T>(pred, option); tChild != nullptr) {
                    return tChild;
                }
            }
        }

        // step 3: Find direct children of the
        // current object and its scene if specified
        if (option == FindOption::DirectThisAndScene && !isScene()) {
            if (auto scene = getParentSceneAsObject(); scene != nullptr) {
                return scene->findChild<T>(pred, FindOption::Direct);
            }
        }

        return nullptr;
    }

    template<typename T>
    std::forward_list<T> findChildren(FindOption option = FindOption::FindRecursively) {
        return findChildren<T>([](T) { return true; }, option);
    }

    template<typename T>
    std::forward_list<T> findChildren(std::string_view name, FindOption option = FindOption::FindRecursively) {
        return findChildren<T>([name](T child) { return child->getName() == name; }, option);
    }

    template<typename T, typename P>
    std::enable_if_t<std::is_invocable_r_v<bool, P, T>, std::forward_list<T>>
    findChildren(P &&pred, FindOption option = FindOption::FindRecursively) {
        Locker locker(m_mutex);

        auto &children = getChildren();
        std::forward_list<T> result;

        // step 1: find direct children
        for (auto child : children) {
            if (auto tChild = child->as<T>(); tChild != nullptr && pred(tChild)) {
                result.emplace_front(tChild);
            }
        }

        // step 2: find recursively if specified
        if (option == FindOption::FindRecursively) {
            for (auto child : children) {
                auto foundChildren = child->findChildren<T>(pred, option);
                result.merge(foundChildren);
            }
        }

        // step 3: Find direct children of the
        // current object and its scene if specified
        if (option == FindOption::DirectThisAndScene && !isScene()) {
            if (auto scene = getParentSceneAsObject(); scene != nullptr) {
                auto foundChildren = scene->findChildren<T>(pred, FindOption::Direct);
                result.merge(foundChildren);
            }
        }

        return result;
    }

    template<typename T>
    static std::string getTypeName() {
        return getTypeName(typeid(T).name());
    }

protected:
    virtual void addChild(Object *child);

    /**
     * Removes a child
     * @param child object to remove
     * @param option
     * @note Once you remove a child, it will no longer have a parent.
     * However, this does not mean that it will be automatically deleted.
     * Therefore, it is important to remember to manually delete such a child.
     * @return true if removed, false otherwise
     */
    virtual bool removeChild(Object *child, FindOption option);

    template<typename T, typename P>
    std::enable_if_t<std::is_invocable_r_v<bool, P, T>, int>
    removeChildren(P &&pred, FindOption option, bool deleteRemoved = true) {
        Locker locker(m_mutex);

        int removedCount = 0;

        // step 1: remove direct children
        auto it_prev = m_children.before_begin();
        auto it = m_children.begin();

        while (it != m_children.end()) {
            auto child = *it;

            if (auto tChild = child->as<T>(); tChild != nullptr && pred(tChild)) {
                it = m_children.erase_after(it_prev);
                ++removedCount;
                tChild->m_parent = nullptr;
                onChildRemoved(tChild);

                if (deleteRemoved) {
                    delete tChild;
                }
            } else {
                ++it_prev;
                ++it;
            }
        }

        // step 2: remove recursively if specified
        if (option == FindOption::FindRecursively) {
            for (auto child : m_children) {
                removedCount += child->removeChildren<T>(pred, option);
            }
        }

        // step 3: Find direct children of the
        // current object and its scene if specified
        if (option == FindOption::DirectThisAndScene && !isScene()) {
            if (auto scene = getParentSceneAsObject(); scene != nullptr) {
                removedCount += scene->removeChildren<T>(pred, FindOption::Direct);
            }
        }

        return removedCount;
    }

    template<typename T>
    int removeChildren(std::string_view name, FindOption option, bool deleteRemoved = true) {
        return removeChildren<T>([name](T child) { return child->getName() == name; }, option, deleteRemoved);
    }

    template<typename T>
    int removeChildren(FindOption option, bool deleteRemoved = true) {
        return removeChildren<T>([](T) { return true; }, option, deleteRemoved);
    }

    virtual void onChildRemoved(Object *child);

    Object* getParentSceneAsObject() const;

    static Object* defaultParent();

private:
    static std::string getTypeName(std::string_view mangledName);

private:
    Object *m_parent {nullptr};
    std::string m_name;
    ObjectList m_children;

    // std::mutex is heavy object and
    // in many cases we don't need it
    std::unique_ptr<std::mutex> m_mutex {nullptr};

private:
    class Locker {
    public:
        explicit Locker(std::unique_ptr<std::mutex> &mutex);
        ~Locker();

    private:
        std::unique_ptr<std::mutex> &m_mutex;
    };

#ifdef ALGINE_SECURE_OPERATIONS
public:
    /**
     * @warning This struct is for internal purposes only
     */
    struct SOPAllocSummary {
        std::atomic_int alloc;
        std::atomic_int dealloc;
    };

    /**
     * <!-- #solgen #ignore -->
     * @warning For internal purposes only. Do not use this function
     */
    static const SOPAllocSummary& _getSOPAllocSummary() {
        return _m_sopAllocSummary;
    }

private:
    inline static SOPAllocSummary _m_sopAllocSummary {0, 0};
#endif
};
}

#endif //ALGINE_OBJECT_H
