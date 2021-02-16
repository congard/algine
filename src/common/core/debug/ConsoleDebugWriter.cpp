#include <algine/core/debug/ConsoleDebugWriter.h>

#include <algine/core/log/Log.h>

namespace algine {
void ConsoleDebugWriter::begin() {
    m_logger = &Log::info("ConsoleDebugWriter");
}

void ConsoleDebugWriter::end() {
    (*m_logger) << Log::end;
}

std::ostream& ConsoleDebugWriter::stream() {
    return m_logger->stream();
}
}
