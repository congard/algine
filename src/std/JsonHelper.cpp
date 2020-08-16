#include <algine/std/JsonHelper.h>

#include <utility>

using namespace std;
using namespace nlohmann;

namespace algine {
JsonHelper::JsonHelper(::json inJson): json(move(inJson)) {
    // see initializer list above
}

JsonHelper::JsonHelper() = default;

void JsonHelper::set(const ::json &inJson) {
    json = inJson;
}

nlohmann::json &JsonHelper::get() {
    return json;
}

string JsonHelper::toString(int indent) const {
    return json.dump(indent);
}
}
