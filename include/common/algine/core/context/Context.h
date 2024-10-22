#ifndef ALGINE_CONTEXT_H
#define ALGINE_CONTEXT_H

#include <algine/platform.h>
#include <tulz/observer/Subject.h>

namespace algine {
class ContextConfig;

/**
 * Note: due to platform limitations, functions
 * <b>create() and destroy() must be called from
 * the main thread</b>, otherwise your app may crash.
 * <br><br>
 * However, these functions can be called from
 * any thread on Android, but due to cross-platform
 * considerations, it is highly recommended to call
 * these functions only from the main thread.
 * <br><br>
 * <ul>
 * References:
 * <ol>
 * <li>https://www.glfw.org/docs/3.3/context_guide.html</li>
 * <li>https://www.glfw.org/docs/3.3/group__window.html#ga5c336fddf2cbb5b92f65f10fb6043344</li>
 * <li>https://doc.qt.io/qt-6/qoffscreensurface.html</li>
 * </ol>
 * </ul>
 */
class AL_EXPORT Context {
public:
    using DestroySubject = tulz::Subject<Context*>;

public:
    /**
     * Creates offscreen context
     * @param config
     * @return
     */
    bool create(const ContextConfig &config);
    bool create(const Context &parent);
    bool create();

    bool destroy();

    bool makeCurrent() const;
    bool detach() const;

    bool isCurrent() const;
    bool isInitialized() const;

    void* getNative() const;

    static Context getCurrent();
    static void* getCurrentNative();

    static DestroySubject::Subscription_t
    addOnDestroyListener(DestroySubject::ObserverAutoPtr_t observer);

#ifdef ALGINE_QT_PLATFORM
    void *m_context {nullptr};
    void *m_surface {nullptr};
#elif !defined(__ANDROID__)
    void *m_context {nullptr};
#else
    void *m_context {nullptr};
    void *m_display {nullptr};
    void *m_surface {nullptr};
#endif

private:
    bool createImpl(int major, int minor, const ContextConfig &config);

private:
    static DestroySubject& getOnDestroy();
};
}

#endif //ALGINE_CONTEXT_H
