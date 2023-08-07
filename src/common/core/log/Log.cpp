#include <algine/core/log/Log.h>

namespace algine {
// Android inserts prefixes (I/, E/ etc) automatically

#ifdef __ANDROID__
    #include <android/log.h>
    #define LOG(tag, priority, format, ...) __android_log_print(priority, tag.c_str(), format, __VA_ARGS__)
    #define LOG_INFO(tag, format, ...) LOG(tag, ANDROID_LOG_INFO, format, __VA_ARGS__)
    #define LOG_ERROR(tag, format, ...) LOG(tag, ANDROID_LOG_ERROR, format, __VA_ARGS__)
    #define LOG_VERBOSE(tag, format, ...) LOG(tag, ANDROID_LOG_VERBOSE, format, __VA_ARGS__)
    #define LOG_INFO_TAG(tag) tag
    #define LOG_ERROR_TAG(tag) tag
    #define LOG_VERBOSE_TAG(tag) tag
#else
    #include <cstdio>
    #define LOG(tag, stream, format, ...) fprintf(stream, (tag + ": " format "\n").c_str(), __VA_ARGS__)
    #define LOG_INFO(tag, format, ...) LOG(tag, stdout, format, __VA_ARGS__)
    #define LOG_ERROR(tag, format, ...) LOG(tag, stderr, format, __VA_ARGS__)
    #define LOG_VERBOSE(tag, format, ...) LOG(tag, stdout, format, __VA_ARGS__)
    #define LOG_INFO_TAG(tag) ("I/" + (tag))
    #define LOG_ERROR_TAG(tag) ("E/" + (tag))
    #define LOG_VERBOSE_TAG(tag) ("V/" + (tag))
#endif

void Log::InputEndListener::onInputEnd(Logger &logger) {
    auto &tag = logger.getTag();
    auto log = logger.str();

    switch (logger.getType()) {
        case Logger::Type::Info:
            Log::info(tag, log);
            break;
        case Logger::Type::Error:
            Log::error(tag, log);
            break;
        case Logger::Type::Verbose:
            Log::verbose(tag, log);
            break;
    }
}

std::mutex Log::m_mutex;
Log::InputEndListener Log::m_endListener;

decltype(Log::m_disabledLoggers) Log::m_disabledLoggers {0};

Logger Log::info(const std::string &tag) {
    return {Logger::Type::Info, tag, &m_endListener};
}

Logger Log::error(const std::string &tag) {
    return {Logger::Type::Error, tag, &m_endListener};
}

Logger Log::verbose(const std::string &tag) {
    return {Logger::Type::Verbose, tag, &m_endListener};
}

void Log::info(const std::string &tag, std::string_view str) {
    if (isEnabled(Logger::Type::Info)) {
        std::lock_guard lockGuard(m_mutex);
        LOG_INFO(LOG_INFO_TAG(tag), "%s", str.data());
    }
}

void Log::error(const std::string &tag, std::string_view str) {
    if (isEnabled(Logger::Type::Error)) {
        std::lock_guard lockGuard(m_mutex);
        LOG_ERROR(LOG_ERROR_TAG(tag), "%s", str.data());
    }
}

void Log::verbose(const std::string &tag, std::string_view str) {
    if (isEnabled(Logger::Type::Verbose)) {
        std::lock_guard lockGuard(m_mutex);
        LOG_VERBOSE(LOG_VERBOSE_TAG(tag), "%s", str.data());
    }
}

void Log::setEnabled(Logger::Type type, bool enabled) {
    m_disabledLoggers[static_cast<std::size_t>(type)] = !enabled;
}

bool Log::isEnabled(Logger::Type type) {
    return !m_disabledLoggers[static_cast<std::size_t>(type)];
}
}
