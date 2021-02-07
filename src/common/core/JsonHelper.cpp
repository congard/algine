#include <algine/core/JsonHelper.h>

#include <utility>

using namespace std;

namespace algine {
JsonHelper::JsonHelper(nlohmann::json inJson)
    : json(move(inJson))
{
    // see initializer list above
}

JsonHelper::JsonHelper(const string &strJson) {
    json = nlohmann::json::parse(strJson);
}

JsonHelper::JsonHelper() = default;

void JsonHelper::set(const nlohmann::json &inJson) {
    json = inJson;
}

void JsonHelper::append(const JsonHelper &jsonHelper) {
    if (!jsonHelper.json.empty()) {
        json.merge_patch(jsonHelper.json);
    }
}

bool JsonHelper::empty() const {
    return json.empty();
}

nlohmann::json& JsonHelper::get() {
    return json;
}

string JsonHelper::toString(int indent) const {
    return json.dump(indent);
}
}
