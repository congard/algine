#ifndef ALGINE_LOGGER_H
#define ALGINE_LOGGER_H

#include <algine/platform.h>

#include <string>
#include <sstream>
#include <atomic>

namespace algine {
/**
 * Used to print logs of different types.
 * GLM logging support can be enabled by including
 * `algine/core/log/logger/glm.h`.
 * @note Logs will be printed when the instance of
 * Logger is being destroyed. However, you can do
 * it immediately by using `Log::endl`
 * @see `Log`, `Logger& Logger::operator\<\<(endl_t)`
 */
class AL_EXPORT Logger {
public:
    class InputEndListener {
    public:
        virtual void onInputEnd(Logger &logger) = 0;
    };

    enum class Type {
        Verbose,
        Debug,
        Info,
        Warn,
        Error
    };

    struct endl_t {};

public:
    Logger() = default;
    Logger(Type type, std::string tag, InputEndListener *listener);
    Logger(Logger &&other) noexcept;
    Logger& operator=(Logger &&other) noexcept;
    ~Logger();

    Logger& operator<<(bool val);
    Logger& operator<<(const std::string &val);

    /**
     * Prints immediately.
     * @return Reference to the current logger.
     */
    Logger& operator<<(endl_t);

    template<typename T>
    std::enable_if_t<std::is_pointer_v<T>, Logger&>
    operator<<(T ptr) {
        m_stream << ptr;
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>, Logger&>
    operator<<(T num) {
        m_stream << num;
        return *this;
    }

    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, Logger&>
    operator<<(const std::atomic<T> &val) {
        *this << val.load();
        return *this;
    }

    void setTag(const std::string &tag);
    void setType(Type type);
    void setInputEndListener(InputEndListener *listener);

    const std::string& getTag() const;
    Type getType() const;

    std::string str() const;

private:
    void flush();

private:
    Type m_type {};
    std::string m_tag;
    std::ostringstream m_stream;
    InputEndListener *m_endListener {};
};
}


#endif //ALGINE_LOGGER_H
