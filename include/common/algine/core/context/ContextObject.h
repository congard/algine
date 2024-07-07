#ifndef ALGINE_CONTEXTOBJECT_H
#define ALGINE_CONTEXTOBJECT_H

#include <algine/core/Object.h>
#include <algine/core/Engine.h>
#include <algine/platform.h>
#include <tulz/util/static_initializer.h>

#ifdef ALGINE_SECURE_OPERATIONS
    #include <algine/core/context/Context.h>
    #include <unordered_map>
    #include <mutex>

    #if ALGINE_SOP_LEVEL == 0
        #include <algine/core/log/Log.h>
    #endif
#endif

namespace algine {
/**
 * <ul>
 *   <li>A context object is an object that belongs to some context;</li>
 *   <li>Context objects have an id;</li>
 *   <li>A default object exists for each non-abstract class that derives
 *       from ContexObject;</li>
 *   <li>If defined, additional checks will be applied for each operation
 *       that changes object's state;</li>
 * </ul>
 * @note normally you don't need to use this class directly,
 * this class is for internal use only
 * @example some context objects: Framebuffer, Renderbuffer, TextureCube,
 * Texture2D, ShaderProgram etc.
 */
class AL_EXPORT ContextObject: public Object {
public:
    uint getId() const {
        return m_id;
    }

protected:
    explicit ContextObject(Object *parent = defaultParent()): Object(parent) {}
    ~ContextObject() override = default;

protected:
    uint m_id {0};

protected:
#ifdef ALGINE_SECURE_OPERATIONS
    virtual void checkBinding() const = 0;
    virtual void commitBinding() const = 0;
    virtual void commitUnbinding() const = 0;
#else
    constexpr void checkBinding() const {}
    constexpr void commitBinding() const {}
    constexpr void commitUnbinding() const {}
#endif
};

/**
 * Stores data such as the default object
 * or the currently bound object.
 * @note this class is for internal use only
 * @tparam T context object type
 */
template<typename T>
class ContextObjectImpl {
public:
    static T* getDefault() {
        return m_default.obj;
    }

private:
    inline static struct {
        T *obj;
    } m_default;

#ifdef ALGINE_SECURE_OPERATIONS
public:
    static void throwError(std::string_view message) {
#if ALGINE_SOP_LEVEL == 0
        Log::error("AlgineSOP", message);
#elif ALGINE_SOP_LEVEL == 1
        throw std::runtime_error(message.data());
#else
#error "Unknown SOP level"
#endif
    };

    static void checkBinding(const T *self) {
        if (getBound() == self)
            return;

        std::string message = "Non-bonded ";
        message.append(Object::getTypeName<T>());
        message.append(" (id " + std::to_string(self->getId()) + ") state change\n");

        throwError(message);
    };

    static void commitBinding(const T *self) {
        m_bound.set(self);
    }

    static void commitUnbinding() {
        m_bound.set(nullptr);
    }

    static const T* getBound() {
        return m_bound.get();
    }

private:
    inline static class {
    public:
        inline void set(const T *val) {
            std::lock_guard guard(mutex);
            value[Context::getCurrentNative()] = val;
        }

        inline const T* get() {
            std::lock_guard guard(mutex);

            if (auto it = value.find(Context::getCurrentNative()); it != value.end()) {
                return (*it).second;
            } else {
                return getDefault();
            }
        }

        inline void erase(Context *context) {
            std::lock_guard guard(mutex);
            value.erase(context->getNative());
        }

    private:
        std::unordered_map<void*, const T*> value; // pair<native context, object>
        std::mutex mutex;
    } m_bound {};

    INLINE_STATIC_INITIALIZER({
        Context::addOnDestroyListener([](Context *context) {
            m_bound.erase(context);
        });
    })
#endif
};
} // algine

/**
 * Every base context object must be defined using this macro
 */
#define AL_CONTEXT_OBJECT(__name) \
class AL_EXPORT __name: public algine::ContextObject

#ifdef ALGINE_SECURE_OPERATIONS
#define _AL_CONTEXT_OBJECT_SOP(__name) \
protected: \
    inline void checkBinding() const override { algine::ContextObjectImpl<__name>::checkBinding(this); } \
    inline void commitBinding() const override { algine::ContextObjectImpl<__name>::commitBinding(this); } \
    inline void commitUnbinding() const override { algine::ContextObjectImpl<__name>::commitUnbinding(); }
#else
#define _AL_CONTEXT_OBJECT_SOP(__name)
#endif

/**
 * Every non-abstract context object must have this macro, e.g.:
 * <pre>
 * AL_CONTEXT_OBJECT(MyContextObject) {
 *     AL_CONTEXT_OBJECT_IMPL(MyContextObject)
 * public:
 *     // ...
 * </pre>
 * }
 */
#define AL_CONTEXT_OBJECT_IMPL(__name) \
_AL_CONTEXT_OBJECT_SOP(__name) \
private: \
    friend class algine::ContextObjectImpl<__name>; \
    explicit __name(std::false_type) {}  /* constructor for default context object */ \
public: \
    inline static __name* getDefault() { return algine::ContextObjectImpl<__name>::getDefault(); }

// for listening context destruction
#ifdef ALGINE_SECURE_OPERATIONS
#define _AL_CONTEXT_OBJECT_SOP_INITIALIZER(__name) \
template<> STATIC_INITIALIZER_DEF(algine::ContextObjectImpl<__name>)
#else
#define _AL_CONTEXT_OBJECT_SOP_INITIALIZER(__name)
#endif

/**
 * Every non-abstract context object must use this macro in
 * the source file, e.g.:
 * <pre>AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(MyContextObject) { .obj = new MyContextObject() }</pre>
 */
#define AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(__name) \
_AL_CONTEXT_OBJECT_SOP_INITIALIZER(__name) \
template<> decltype(algine::ContextObjectImpl<__name>::m_default) algine::ContextObjectImpl<__name>::m_default

#endif //ALGINE_CONTEXTOBJECT_H
