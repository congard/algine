#ifndef ALGINE_CONSOLEDEBUGWRITER_H
#define ALGINE_CONSOLEDEBUGWRITER_H

#include <algine/core/window/DebugWriter.h>

#include <algine/core/log/Logger.h>

namespace algine {
class ConsoleDebugWriter: public DebugWriter {
public:
    void begin() override;
    void end() override;

    std::ostream& stream() override;

private:
    Logger *m_logger;
};
}

#endif //ALGINE_CONSOLEDEBUGWRITER_H
