#ifndef ALGINE_CONSOLEDEBUGWRITER_H
#define ALGINE_CONSOLEDEBUGWRITER_H

#include <algine/core/debug/DebugWriter.h>

namespace algine {
class ConsoleDebugWriter: public DebugWriter {
public:
    Logger logger() override;

private:
    void onInputEnd(Logger &logger) override;
};
}

#endif //ALGINE_CONSOLEDEBUGWRITER_H
