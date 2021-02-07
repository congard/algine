#include <algine/core/Object.h>

#include <algine/core/JsonHelper.h>

using namespace nlohmann;
using namespace std;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Name, "name");
}

namespace algine {
void Object::setName(const string &inName) {
    name = inName;
}

string Object::getName() const {
    return name;
}

void Object::import(const JsonHelper &jsonHelper) {
    name = jsonHelper.readValue<string>(Config::Name);
}

JsonHelper Object::dump() {
    json config;

    config[Config::Name] = name;

    return config;
}
}
