#include <algine/core/log/Log.h>
#include <algine/core/log/DefaultLogWriter.h>

namespace algine {
void Log::InputEndListener::onInputEnd(Logger &logger) {
    printLog(logger.getType(), logger.getTag(), logger.str());
}

std::unique_ptr<LogWriter> Log::m_writer {new DefaultLogWriter()};
Log::InputEndListener Log::m_endListener;

decltype(Log::m_disabledLoggers) Log::m_disabledLoggers {0};

Logger Log::verbose(const std::string &tag) {
    return {Logger::Type::Verbose, tag, &m_endListener};
}

Logger Log::debug(const std::string &tag) {
    return {Logger::Type::Debug, tag, &m_endListener};
}

Logger Log::info(const std::string &tag) {
    return {Logger::Type::Info, tag, &m_endListener};
}

Logger Log::warn(const std::string &tag) {
    return {Logger::Type::Warn, tag, &m_endListener};
}

Logger Log::error(const std::string &tag) {
    return {Logger::Type::Error, tag, &m_endListener};
}

void Log::verbose(const std::string &tag, std::string_view str) {
    printLog(Logger::Type::Verbose, tag, str);
}

void Log::debug(const std::string &tag, std::string_view str) {
    printLog(Logger::Type::Debug, tag, str);
}

void Log::info(const std::string &tag, std::string_view str) {
    printLog(Logger::Type::Info, tag, str);
}

void Log::warn(const std::string &tag, std::string_view str) {
    printLog(Logger::Type::Warn, tag, str);
}

void Log::error(const std::string &tag, std::string_view str) {
    printLog(Logger::Type::Error, tag, str);
}

void Log::setEnabled(Logger::Type type, bool enabled) {
    m_disabledLoggers[static_cast<std::size_t>(type)] = !enabled;
}

bool Log::isEnabled(Logger::Type type) {
    return !m_disabledLoggers[static_cast<std::size_t>(type)];
}

void Log::printLog(Logger::Type type, const std::string &tag, std::string_view str) {
    if (!isEnabled(type) || m_writer == nullptr)
        return;

    switch (type) {
        case Logger::Type::Verbose:
            m_writer->verbose(tag, str);
            break;
        case Logger::Type::Debug:
            m_writer->debug(tag, str);
            break;
        case Logger::Type::Info:
            m_writer->info(tag, str);
            break;
        case Logger::Type::Warn:
            m_writer->warn(tag, str);
            break;
        case Logger::Type::Error:
            m_writer->error(tag, str);
            break;
    }
}

void Log::setWriter(std::unique_ptr<LogWriter> writer) {
    m_writer = std::move(writer);
}

const std::unique_ptr<LogWriter>& Log::getWriter() {
    return m_writer;
}
}
