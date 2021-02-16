#include <algine/core/log/Logger.h>

#include <algine/core/log/Log.h>

#ifdef __ANDROID__
    #include <android/log.h>

    #define LOG(format, ...) __android_log_print(m_priority, m_tag.c_str(), format, __VA_ARGS__)
#else
    #include <cstdio>

    #define LOG(format, ...) fprintf((FILE*) m_output, (m_tag + ": " format "\n").c_str(), __VA_ARGS__)
#endif

#include <cstring>

namespace algine {
Logger::Logger(Type type) {
    if (type == Type::Info) {
        enable_if_desktop(m_output = stdout);
        enable_if_android(m_priority = ANDROID_LOG_INFO);
    } else {
        enable_if_desktop(m_output = stderr);
        enable_if_android(m_priority = ANDROID_LOG_ERROR);
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
    if (strcmp(val, Log::end) == 0) {
        LOG("%s", m_stream.str().c_str());
        m_stream = std::ostringstream();
    } else {
        m_stream << val;
    }

    return *this;
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

std::ostream& Logger::stream() {
    return m_stream;
}
}
