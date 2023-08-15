#include <algine/core/log/Logger.h>

#include <utility>

namespace algine {
Logger::Logger(Type type, std::string tag, InputEndListener *listener)
    : m_type(type),
      m_tag(std::move(tag)),
      m_endListener(listener) {}

Logger::Logger(Logger &&other) noexcept {
    std::swap(m_type, other.m_type);
    std::swap(m_tag, other.m_tag);
    std::swap(m_stream, other.m_stream);
    std::swap(m_endListener, other.m_endListener);
}

Logger& Logger::operator=(Logger &&other) noexcept {
    if (this == &other)
        return *this;

    std::swap(*this, other);

    return *this;
}

Logger::~Logger() {
    flush();
}

#define writeLog(value) m_stream << (value); return *this

Logger& Logger::operator<<(bool val) {
    writeLog(val ? "true" : "false");
}

Logger& Logger::operator<<(short val) {
    writeLog(val);
}

Logger& Logger::operator<<(unsigned short val) {
    writeLog(val);
}

Logger& Logger::operator<<(int val) {
    writeLog(val);
}

Logger& Logger::operator<<(unsigned int val) {
    writeLog(val);
}

Logger& Logger::operator<<(long val) {
    writeLog(val);
}

Logger& Logger::operator<<(unsigned long val) {
    writeLog(val);
}

Logger& Logger::operator<<(float val) {
    writeLog(val);
}

Logger& Logger::operator<<(double val) {
    writeLog(val);
}

Logger& Logger::operator<<(long double val) {
    writeLog(val);
}

Logger& Logger::operator<<(const std::string &val) {
    writeLog(val);
}

Logger& Logger::operator<<(algine::Logger::endl_t) {
    flush();
    return *this;
}

void Logger::setTag(const std::string &tag) {
    m_tag = tag;
}

void Logger::setType(Type type) {
    m_type = type;
}

void Logger::setInputEndListener(InputEndListener *listener) {
    m_endListener = listener;
}

const std::string& Logger::getTag() const {
    return m_tag;
}

Logger::Type Logger::getType() const {
    return m_type;
}

std::string Logger::str() const {
    return m_stream.str();
}

void Logger::flush() {
    if (m_endListener && !str().empty()) {
        m_endListener->onInputEnd(*this);
    }

    m_stream.str({});
}
}
