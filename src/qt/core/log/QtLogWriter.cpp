#include <algine/core/log/QtLogWriter.h>

#include <QtGlobal>

#define LOG_C_STR(prefix, tag, format) (prefix + (tag) + ": " + format).c_str()
#define LOG_VERBOSE(tag, format, ...) qInfo(LOG_C_STR("V/", tag, format), __VA_ARGS__)
#define LOG_DEBUG(tag, format, ...) qDebug(LOG_C_STR("D/", tag, format), __VA_ARGS__)
#define LOG_INFO(tag, format, ...) qInfo(LOG_C_STR("I/", tag, format), __VA_ARGS__)
#define LOG_WARN(tag, format, ...) qWarning(LOG_C_STR("W/", tag, format), __VA_ARGS__)
#define LOG_ERROR(tag, format, ...) qCritical(LOG_C_STR("E/", tag, format), __VA_ARGS__)

namespace algine {
void QtLogWriter::verbose(const std::string &tag, std::string_view str) {
    LOG_VERBOSE(tag, "\033[0;36m%s\033[0m", str.data());
}

void QtLogWriter::debug(const std::string &tag, std::string_view str) {
    LOG_DEBUG(tag, "\033[0;37m%s\033[0m", str.data());
}

void QtLogWriter::info(const std::string &tag, std::string_view str) {
    LOG_INFO(tag, "%s", str.data());
}

void QtLogWriter::warn(const std::string &tag, std::string_view str) {
    LOG_WARN(tag, "\033[0;33m%s\033[0m", str.data());
}

void QtLogWriter::error(const std::string &tag, std::string_view str) {
    LOG_ERROR(tag, "%s", str.data());
}
} // algine