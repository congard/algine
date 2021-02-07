#ifndef ALGINE_DEBUGWRITER_H
#define ALGINE_DEBUGWRITER_H

#include <ostream>

namespace algine {
class DebugWriter {
public:
    virtual ~DebugWriter() = default;

    virtual std::ostream& stream() = 0;
};
}

#endif //ALGINE_DEBUGWRITER_H
