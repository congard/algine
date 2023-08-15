#ifndef ALGINE_LOG_H
#define ALGINE_LOG_H

#include <algine/core/log/Logger.h>
#include <algine/core/log/LogWriter.h>

#include <mutex>
#include <bitset>
#include <memory>

namespace algine {
class Log {
public:
    static Logger verbose(const std::string &tag = "Algine");
    static Logger debug(const std::string &tag = "Algine");
    static Logger info(const std::string &tag = "Algine");
    static Logger warn(const std::string &tag = "Algine");
    static Logger error(const std::string &tag = "Algine");

    static void verbose(const std::string &tag, std::string_view str);
    static void debug(const std::string &tag, std::string_view str);
    static void info(const std::string &tag, std::string_view str);
    static void warn(const std::string &tag, std::string_view str);
    static void error(const std::string &tag, std::string_view str);

    static void setEnabled(Logger::Type type, bool enabled);
    static bool isEnabled(Logger::Type type);

    /// #solgen #ignore
    static void setWriter(std::unique_ptr<LogWriter> writer);

    /// #solgen #ignore
    static const std::unique_ptr<LogWriter>& getWriter();

public:
    constexpr static Logger::endl_t endl;

private:
    Log() = default;

    static void printLog(Logger::Type type, const std::string &tag, std::string_view str);

private:
    class InputEndListener: public Logger::InputEndListener {
    public:
        void onInputEnd(Logger &logger) override;
    };

    static std::unique_ptr<LogWriter> m_writer;
    static InputEndListener m_endListener;

    static std::bitset<5> m_disabledLoggers;
};
}


#endif //ALGINE_LOG_H
