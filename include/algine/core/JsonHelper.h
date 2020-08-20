#ifndef ALGINE_JSONHELPER_H
#define ALGINE_JSONHELPER_H

#include <nlohmann/json.hpp>

namespace algine {
class JsonHelper {
public:
    static constexpr int DoNotFormat = -1;

public:
    // do not mark explicit because we really need implicit conversion
    // just ignore Clang-Tidy warning
    JsonHelper(nlohmann::json inJson);
    JsonHelper(const std::string &strJson);
    JsonHelper();

    void set(const nlohmann::json &inJson);
    nlohmann::json& get();

    std::string toString(int indent = 4) const;

public:
    nlohmann::json json;
};
}

#endif //ALGINE_JSONHELPER_H
