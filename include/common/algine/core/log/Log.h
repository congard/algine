#ifndef ALGINE_LOG_H
#define ALGINE_LOG_H

#include <algine/core/log/Logger.h>
#include <mutex>
#include <bitset>

namespace algine {
class Log {
public:
    static Logger info(const std::string &tag = "Algine");
    static Logger error(const std::string &tag = "Algine");
    static Logger verbose(const std::string &tag = "Algine");

    static void info(const std::string &tag, std::string_view str);
    static void error(const std::string &tag, std::string_view str);
    static void verbose(const std::string &tag, std::string_view str);

    static void setEnabled(Logger::Type type, bool enabled);
    static bool isEnabled(Logger::Type type);

private:
    Log() = default;

private:
    class InputEndListener: public Logger::InputEndListener {
    public:
        void onInputEnd(Logger &logger) override;
    };

    static std::mutex m_mutex;
    static InputEndListener m_endListener;

    static std::bitset<3> m_disabledLoggers;
};
}


#endif //ALGINE_LOG_H
