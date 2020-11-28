#ifndef ALGINE_GLMTRANSFERRER_H
#define ALGINE_GLMTRANSFERRER_H

#include <algine/core/JsonHelper.h>

#include <glm/matrix.hpp>

namespace algine::GLMTransferrer {
template<typename T>
JsonHelper dump(const T &glmType) {
    using value_type = typename T::value_type;

    const glm::length_t length = sizeof(T) / sizeof(value_type);

    auto array = reinterpret_cast<const value_type *>(&glmType);
    nlohmann::json values;

    for (glm::length_t i = 0; i < length; i++)
        values[i] = array[i];

    return values;
}

template<typename T>
T import(const JsonHelper &jsonHelper) {
    const auto &config = jsonHelper.json;

    if (config.empty())
        return T();

    T glmType;
    auto array = reinterpret_cast<typename T::value_type *>(&glmType);

    for (glm::length_t i = 0; i < sizeof(T) / sizeof(array[0]); i++)
        array[i] = config[i];

    return glmType;
}
}

#endif //ALGINE_GLMTRANSFERRER_H
