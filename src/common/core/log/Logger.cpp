#include <algine/core/log/Logger.h>

#ifdef __ANDROID__
    #include <android/log.h>

    #define LOG(format, ...) __android_log_print(m_priority, m_tag.c_str(), format, __VA_ARGS__)
#else
    #include <cstdio>

    #define LOG(format, ...) fprintf((FILE*) m_stream, (m_tag + ": " format "\n").c_str(), __VA_ARGS__)
#endif

namespace algine {
Logger::Logger(Type type) {
    if (type == Type::Info) {
        enable_if_desktop(m_stream = stdout);
        enable_if_android(m_priority = ANDROID_LOG_INFO);
    } else {
        enable_if_desktop(m_stream = stderr);
        enable_if_android(m_priority = ANDROID_LOG_ERROR);
    }
}

Logger& Logger::operator<<(bool val) {
    if (val) {
        LOG("%s", "true");
    } else {
        LOG("%s", "false");
    }

    return *this;
}

Logger& Logger::operator<<(short val) {
    LOG("%i", val);

    return *this;
}

Logger& Logger::operator<<(unsigned short val) {
    LOG("%u", val);

    return *this;
}

Logger& Logger::operator<<(int val) {
    LOG("%i", val);

    return *this;
}

Logger& Logger::operator<<(unsigned int val) {
    LOG("%u", val);

    return *this;
}

Logger& Logger::operator<<(long val) {
    LOG("%li", val);

    return *this;
}

Logger& Logger::operator<<(unsigned long val) {
    LOG("%lu", val);

    return *this;
}

Logger& Logger::operator<<(float val) {
    LOG("%f", val);

    return *this;
}

Logger& Logger::operator<<(double val) {
    LOG("%f", val);

    return *this;
}

Logger& Logger::operator<<(long double val) {
    LOG("%Lf", val);

    return *this;
}

Logger& Logger::operator<<(const char *val) {
    LOG("%s", val);

    return *this;
}

Logger& Logger::operator<<(void *val) {
    LOG("%p", val);

    return *this;
}

Logger& Logger::operator<<(const std::string &val) {
    LOG("%s", val.c_str());

    return *this;
}

void Logger::setTag(const std::string &tag) {
    m_tag = tag;
}
}
