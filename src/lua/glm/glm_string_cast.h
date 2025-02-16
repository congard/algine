#ifndef ALGINE_GLM_STRING_CAST_H
#define ALGINE_GLM_STRING_CAST_H

#include <string>

// Workaround for sol
// Unfortunately, we cannot include glm/gtx/string_cast.hpp directly to GLMLuaTypes

namespace algine {
template<typename T>
std::string glm_to_string(const T &type);
}

#endif //ALGINE_GLM_STRING_CAST_H
