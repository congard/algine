#ifndef ALGINE_CONTEXT_H
#define ALGINE_CONTEXT_H

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
class Context {
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

    static Context getCurrent();
    static void* getCurrentNative();

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
};
}

#endif //ALGINE_CONTEXT_H
