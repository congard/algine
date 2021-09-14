#ifndef ALGINE_DEBUGWRITER_H
#define ALGINE_DEBUGWRITER_H

#include <algine/core/log/Logger.h>

namespace algine {
class DebugWriter: protected Logger::InputEndListener {
public:
    virtual ~DebugWriter() = default;

    virtual Logger logger() = 0;
};
}

#endif //ALGINE_DEBUGWRITER_H
