#ifndef ALGINE_QTLOGWRITER_H
#define ALGINE_QTLOGWRITER_H

#include <algine/core/log/LogWriter.h>

namespace algine {
class QtLogWriter: public LogWriter {
public:
    void verbose(const std::string &tag, std::string_view str) override;
    void debug(const std::string &tag, std::string_view str) override;
    void info(const std::string &tag, std::string_view str) override;
    void warn(const std::string &tag, std::string_view str) override;
    void error(const std::string &tag, std::string_view str) override;
};
} // algine

#endif //ALGINE_QTLOGWRITER_H
