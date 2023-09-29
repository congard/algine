#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/core/shader/ShaderProgram.h>

namespace algine {
InputLayoutShapeLocations::InputLayoutShapeLocations(ShaderProgram *program,
    const std::map<std::string, std::string> &locations)
{
    load(program, locations);
}

int InputLayoutShapeLocations::operator[](const std::string &name) const {
    if (auto it = data.find(name); it != data.end())
        return it->second;
    return None;
}

int& InputLayoutShapeLocations::operator[](const std::string &name) {
    return data[name];
}

void
InputLayoutShapeLocations::load(ShaderProgram *program, const std::map<std::string, std::string> &locations) {
    for (const auto &p : locations) {
        program->loadUniformLocation(p.second);
        data[p.first] = program->getLocation(p.second);
    }
}

int InputLayoutShapeLocations::get(const std::string &name) const {
    return operator[](name);
}

void InputLayoutShapeLocations::set(const std::string &name, int value) {
    data[name] = value;
}

InputLayoutShapeLocations InputLayoutShapeLocations::fromFile(const std::string &path) {
    InputLayoutShapeLocations locations;
    locations.execute(path);
    return locations;
}

InputLayoutShapeLocations InputLayoutShapeLocations::fromScript(const std::string &script) {
    InputLayoutShapeLocations locations;
    locations.executeString(script);
    return locations;
}

void InputLayoutShapeLocations::exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
    exec_t<InputLayoutShapeLocations>(s, path, lua, tenv);
}
}
