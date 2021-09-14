#include <algine/core/debug/FileDebugWriter.h>
#include <algine/core/Engine.h>

namespace algine {
std::mutex FileDebugWriter::m_mutex;

FileDebugWriter::FileDebugWriter(const std::string &file) {
    m_stream.open(file, std::ios::app);
}

FileDebugWriter::~FileDebugWriter() {
    m_stream.flush();
    m_stream.close();
}

Logger FileDebugWriter::logger() {
    Logger logger;
    logger.setInputEndListener(this);
    return logger;
}

void FileDebugWriter::onInputEnd(Logger &logger) {
    std::lock_guard lockGuard(m_mutex);
    m_stream << "[" << Engine::time() << "] FileDebugWriter: " << logger.str() << "\n";
}
}
