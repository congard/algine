#ifndef ALGINE_CONTEXT_H
#define ALGINE_CONTEXT_H

namespace algine {
#ifndef __ANDROID__
struct Context {
    void *context {nullptr};
};
#else
struct Context {
    void *context {nullptr};
    void *display {nullptr};
    void *surface {nullptr};
};
#endif
}

#endif //ALGINE_CONTEXT_H
