/**
 * Overloaded `Logger` operators to
 * provide `glm` logging support
 */

#ifndef ALGINE_LOGGER_GLM_H
#define ALGINE_LOGGER_GLM_H

#include <algine/core/log/Logger.h>
#include <glm/gtx/string_cast.hpp>

namespace algine {
namespace detail {
template<typename T>
Logger& log_glm(Logger &logger, const T &val) {
    logger << glm::to_string(val);
    return logger;
}
}

template<glm::length_t L, typename T, glm::qualifier Q>
Logger& operator<<(Logger &logger, const glm::vec<L, T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
Logger& operator<<(Logger &logger, const glm::mat<C, R, T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<typename T, glm::qualifier Q>
Logger& operator<<(Logger &logger, const glm::qua<T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<typename T, glm::qualifier Q>
Logger& operator<<(Logger &logger, const glm::tdualquat<T, Q> &val) {
    return detail::log_glm(logger, val);
}
}

#endif //ALGINE_LOGGER_GLM_H
