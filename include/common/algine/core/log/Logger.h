#ifndef ALGINE_LOGGER_H
#define ALGINE_LOGGER_H

#include <algine/templates.h>

#include <string>
#include <sstream>

namespace algine {
class Logger {
public:
    enum class Type {
        Info,
        Error
    };

public:
    explicit Logger(Type type);

    Logger& operator<<(bool val);
    Logger& operator<<(short val);
    Logger& operator<<(unsigned short val);
    Logger& operator<<(int val);
    Logger& operator<<(unsigned int val);
    Logger& operator<<(long val);
    Logger& operator<<(unsigned long val);
    Logger& operator<<(float val);
    Logger& operator<<(double val);
    Logger& operator<<(long double val);
    Logger& operator<<(const char *val);
    Logger& operator<<(void *val);
    Logger& operator<<(const std::string &val);

    void setTag(const std::string &tag);

    std::ostream& stream();

private:
    std::string m_tag;
    std::ostringstream m_stream;

private:
    enable_if_desktop(void *m_output);
    enable_if_android(int m_priority);
};
}


#endif //ALGINE_LOGGER_H
