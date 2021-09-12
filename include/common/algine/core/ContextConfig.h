#ifndef ALGINE_CONTEXTCONFIG_H
#define ALGINE_CONTEXTCONFIG_H

#include <algine/core/Context.h>

namespace algine {
struct ContextConfig {
    Context parent;
    int width = 1;
    int height = 1;
};
}

#endif //ALGINE_CONTEXTCONFIG_H
