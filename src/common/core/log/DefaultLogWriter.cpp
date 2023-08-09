#ifndef __ANDROID__

#include <algine/core/log/DefaultLogWriter.h>

#include <cstdio>

#define LOG(tag, stream, format, ...) fprintf(stream, (tag + ": " format "\n").c_str(), __VA_ARGS__)
#define LOG_VERBOSE(tag, format, ...) LOG("V/" + (tag), stdout, format, __VA_ARGS__)
#define LOG_DEBUG(tag, format, ...) LOG("D/" + (tag), stdout, format, __VA_ARGS__)
#define LOG_INFO(tag, format, ...) LOG("I/" + (tag), stdout, format, __VA_ARGS__)
#define LOG_WARN(tag, format, ...) LOG("W/" + (tag), stdout, format, __VA_ARGS__)
#define LOG_ERROR(tag, format, ...) LOG("E/" + (tag), stderr, format, __VA_ARGS__)

namespace algine {
void DefaultLogWriter::verbose(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_VERBOSE(tag, "\033[0;36m%s\033[0m", str.data());
}

void DefaultLogWriter::debug(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_DEBUG(tag, "\033[0;37m%s\033[0m", str.data());
}

void DefaultLogWriter::info(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_INFO(tag, "%s", str.data());
}

void DefaultLogWriter::warn(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_WARN(tag, "\033[0;33m%s\033[0m", str.data());
}

void DefaultLogWriter::error(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_ERROR(tag, "%s", str.data());
}
} // algine

#endif