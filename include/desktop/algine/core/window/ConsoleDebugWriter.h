#ifndef ALGINE_CONSOLEDEBUGWRITER_H
#define ALGINE_CONSOLEDEBUGWRITER_H

#include <algine/core/window/DebugWriter.h>

namespace algine {
class ConsoleDebugWriter: public DebugWriter {
public:
    std::ostream& stream() override;
};
}

#endif //ALGINE_CONSOLEDEBUGWRITER_H
