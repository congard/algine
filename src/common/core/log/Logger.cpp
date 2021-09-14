#include <algine/core/log/Logger.h>

namespace algine {
Logger::Logger(Logger &&other) noexcept {
    std::swap(m_id, other.m_id);
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
    if (m_endListener) {
        m_endListener->onInputEnd(*this);
    }
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

Logger& Logger::operator<<(const char *val) {
    writeLog(val);
}

Logger& Logger::operator<<(void *val) {
    writeLog(val);
}

Logger& Logger::operator<<(const std::string &val) {
    writeLog(val);
}

void Logger::setTag(const std::string &tag) {
    m_tag = tag;
}

void Logger::setId(int id) {
    m_id = id;
}

void Logger::setInputEndListener(InputEndListener *listener) {
    m_endListener = listener;
}

const std::string& Logger::getTag() const {
    return m_tag;
}

int Logger::getId() const {
    return m_id;
}

std::string Logger::str() const {
    return m_stream.str();
}
}
