#ifndef ALGINE_CONTEXT_H
#define ALGINE_CONTEXT_H

#include <mutex>

namespace algine {
class ContextConfig;

class Context {
public:
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

#ifndef __ANDROID__
    void *m_context {nullptr};

private:
    static std::mutex m_contextMutex;
#else
    void *m_context {nullptr};
    void *m_display {nullptr};
    void *m_surface {nullptr};
#endif
};
}

#endif //ALGINE_CONTEXT_H
