#include "glm_string_cast.h"

#include <glm/gtx/string_cast.hpp>

namespace algine {
template<typename T>
std::string glm_to_string(const T &type) {
    return glm::to_string(type);
}

template std::string glm_to_string<glm::vec2>(const glm::vec2&);
template std::string glm_to_string<glm::vec3>(const glm::vec3&);
template std::string glm_to_string<glm::vec4>(const glm::vec4&);
template std::string glm_to_string<glm::mat2>(const glm::mat2&);
template std::string glm_to_string<glm::mat3>(const glm::mat3&);
template std::string glm_to_string<glm::mat4>(const glm::mat4&);
template std::string glm_to_string<glm::quat>(const glm::quat&);
}
