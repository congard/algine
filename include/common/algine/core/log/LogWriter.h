#ifndef ALGINE_LOGWRITER_H
#define ALGINE_LOGWRITER_H

#include <string>
#include <string_view>

namespace algine {
class LogWriter {
public:
    virtual ~LogWriter() = default;

    virtual void verbose(const std::string &tag, std::string_view str) = 0;
    virtual void debug(const std::string &tag, std::string_view str) = 0;
    virtual void info(const std::string &tag, std::string_view str) = 0;
    virtual void warn(const std::string &tag, std::string_view str) = 0;
    virtual void error(const std::string &tag, std::string_view str) = 0;
};
} // algine

#endif //ALGINE_LOGWRITER_H
