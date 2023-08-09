#ifndef ALGINE_DEFAULTLOGWRITER_H
#define ALGINE_DEFAULTLOGWRITER_H

#include <algine/core/log/LogWriter.h>
#include <mutex>

namespace algine {
class DefaultLogWriter: public LogWriter {
public:
    void verbose(const std::string &tag, std::string_view str) override;
    void debug(const std::string &tag, std::string_view str) override;
    void info(const std::string &tag, std::string_view str) override;
    void warn(const std::string &tag, std::string_view str) override;
    void error(const std::string &tag, std::string_view str) override;

#ifndef __ANDROID__
private:
    std::mutex m_mutex;
#endif
};
} // algine

#endif //ALGINE_DEFAULTLOGWRITER_H
