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
Logger&& log_glm(Logger &logger, const T &val) {
    logger << glm::to_string(val);
    return std::move(logger);
}
}

template<glm::length_t D, typename T, glm::qualifier Q, typename L>
Logger&& operator<<(L &&logger, const glm::vec<D, T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q, typename L>
Logger&& operator<<(L &&logger, const glm::mat<C, R, T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<typename T, glm::qualifier Q, typename L>
Logger&& operator<<(L &&logger, const glm::qua<T, Q> &val) {
    return detail::log_glm(logger, val);
}

template<typename T, glm::qualifier Q, typename L>
Logger&& operator<<(L &&logger, const glm::tdualquat<T, Q> &val) {
    return detail::log_glm(logger, val);
}
}

#endif //ALGINE_LOGGER_GLM_H
