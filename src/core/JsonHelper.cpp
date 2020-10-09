#include <algine/core/JsonHelper.h>

#include <utility>

using namespace std;
using namespace nlohmann;

namespace algine {
JsonHelper::JsonHelper(::json inJson)
    : json(move(inJson))
{
    // see initializer list above
}

JsonHelper::JsonHelper(const string &strJson) {
    json = json::parse(strJson);
}

JsonHelper::JsonHelper() = default;

void JsonHelper::set(const ::json &inJson) {
    json = inJson;
}

void JsonHelper::append(const JsonHelper &jsonHelper) {
    if (!jsonHelper.json.empty()) {
        json.merge_patch(jsonHelper.json);
    }
}

nlohmann::json &JsonHelper::get() {
    return json;
}

JsonHelper::operator nlohmann::json&() {
    return json;
}

string JsonHelper::toString(int indent) const {
    return json.dump(indent);
}
}
