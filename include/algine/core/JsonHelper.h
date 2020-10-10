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
    void append(const JsonHelper &jsonHelper);
    bool empty() const;
    nlohmann::json& get();

    template<typename T = nlohmann::json>
    inline void readValue(const std::string &key, T &writeTo, const T &defaultValue = T()) const {
        if (json.contains(key)) {
            writeTo = json[key];
        } else {
            writeTo = defaultValue;
        }
    }

    template<typename T = nlohmann::json>
    inline T readValue(const std::string &key, const T &defaultValue = T()) const {
        T value;
        readValue(key, value, defaultValue);

        return value;
    }

    std::string toString(int indent = 4) const;

public:
    nlohmann::json json;
};
}

#endif //ALGINE_JSONHELPER_H
