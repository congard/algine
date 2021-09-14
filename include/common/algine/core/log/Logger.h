#ifndef ALGINE_LOGGER_H
#define ALGINE_LOGGER_H

#include <string>
#include <sstream>

namespace algine {
class Logger {
public:
    class InputEndListener {
    public:
        virtual void onInputEnd(Logger &logger) = 0;
    };

public:
    Logger() = default;
    Logger(Logger &&other) noexcept;
    Logger& operator=(Logger &&other) noexcept;
    ~Logger();

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
    void setId(int id);
    void setInputEndListener(InputEndListener *listener);

    const std::string& getTag() const;
    int getId() const;

    std::string str() const;

private:
    int m_id {0};
    std::string m_tag;
    std::ostringstream m_stream;
    InputEndListener *m_endListener {};
};
}


#endif //ALGINE_LOGGER_H
