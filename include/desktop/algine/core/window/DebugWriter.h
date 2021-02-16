#ifndef ALGINE_DEBUGWRITER_H
#define ALGINE_DEBUGWRITER_H

#include <ostream>

namespace algine {
class DebugWriter {
public:
    virtual ~DebugWriter();

    virtual void begin();
    virtual void end();

    virtual std::ostream& stream() = 0;
};
}

#endif //ALGINE_DEBUGWRITER_H
