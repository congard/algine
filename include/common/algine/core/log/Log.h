#ifndef ALGINE_LOG_H
#define ALGINE_LOG_H

#include <algine/core/log/Logger.h>

#include <mutex>

namespace algine {
class Log {
public:
    static Logger info(const std::string &tag = "Algine");
    static Logger error(const std::string &tag = "Algine");

    static void info(const std::string &tag, std::string_view str);
    static void error(const std::string &tag, std::string_view str);

private:
    class InputEndListener: public Logger::InputEndListener {
    public:
        void onInputEnd(Logger &logger) override;
    };

    static std::mutex m_mutex;
    static InputEndListener m_endListener;
};
}


#endif //ALGINE_LOG_H
