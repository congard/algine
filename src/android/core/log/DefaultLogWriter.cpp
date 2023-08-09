#include <algine/core/log/DefaultLogWriter.h>

#include <android/log.h>

#define LOG(tag, priority, format, ...) __android_log_print(priority, tag.c_str(), format, __VA_ARGS__)
#define LOG_INFO(tag, format, ...) LOG(tag, ANDROID_LOG_INFO, format, __VA_ARGS__)
#define LOG_DEBUG(tag, format, ...) LOG(tag, ANDROID_LOG_DEBUG, format, __VA_ARGS__)
#define LOG_WARN(tag, format, ...) LOG(tag, ANDROID_LOG_WARN, format, __VA_ARGS__)
#define LOG_ERROR(tag, format, ...) LOG(tag, ANDROID_LOG_ERROR, format, __VA_ARGS__)
#define LOG_VERBOSE(tag, format, ...) LOG(tag, ANDROID_LOG_VERBOSE, format, __VA_ARGS__)

namespace algine {
void DefaultLogWriter::verbose(const std::string &tag, std::string_view str) {
    LOG_VERBOSE(tag, "%s", str.data());
}

void DefaultLogWriter::debug(const std::string &tag, std::string_view str) {
    LOG_DEBUG(tag, "%s", str.data());
}

void DefaultLogWriter::info(const std::string &tag, std::string_view str) {
    LOG_INFO(tag, "%s", str.data());
}

void DefaultLogWriter::warn(const std::string &tag, std::string_view str) {
    LOG_WARN(tag, "%s", str.data());
}

void DefaultLogWriter::error(const std::string &tag, std::string_view str) {
    LOG_ERROR(tag, "%s", str.data());
}
} // algine