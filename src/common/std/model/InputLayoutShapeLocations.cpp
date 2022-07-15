#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/core/shader/ShaderProgram.h>

namespace algine {
InputLayoutShapeLocations::InputLayoutShapeLocations(const ShaderProgramPtr &program,
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
InputLayoutShapeLocations::load(const ShaderProgramPtr &program, const std::map<std::string, std::string> &locations) {
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

void InputLayoutShapeLocations::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "InputLayoutShapeLocations"))
        return;

    lua->registerUsertype<Scriptable>(tenv);

    auto index = static_cast<int (InputLayoutShapeLocations::*)(const std::string&) const>(&InputLayoutShapeLocations::operator[]);
    auto new_index = static_cast<int& (InputLayoutShapeLocations::*)(const std::string&)>(&InputLayoutShapeLocations::operator[]);
    auto ctors = sol::constructors<
            InputLayoutShapeLocations(),
            InputLayoutShapeLocations(const ShaderProgramPtr&, const std::map<std::string, std::string>&)>();
    auto usertype = env.new_usertype<InputLayoutShapeLocations>(
            "InputLayoutShapeLocations",
            sol::meta_function::index, index,
            sol::meta_function::new_index, new_index,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable>());
    usertype["None"] = sol::readonly_property(sol::var(None));
    usertype["Position"] = sol::readonly_property(sol::var(Position));
    usertype["TexCoord"] = sol::readonly_property(sol::var(TexCoord));
    usertype["Normal"] = sol::readonly_property(sol::var(Normal));
    usertype["Tangent"] = sol::readonly_property(sol::var(Tangent));
    usertype["Bitangent"] = sol::readonly_property(sol::var(Bitangent));
    usertype["BoneWeights"] = sol::readonly_property(sol::var(BoneWeights));
    usertype["BoneIds"] = sol::readonly_property(sol::var(BoneIds));
    usertype["data"] = &InputLayoutShapeLocations::data;
    usertype["load"] = [](InputLayoutShapeLocations &self,
            const ShaderProgramPtr &program, std::map<std::string, std::string> locations) { self.load(program, locations); };

    // static
    usertype["fromFile"] = &InputLayoutShapeLocations::fromFile;
    usertype["fromScript"] = &InputLayoutShapeLocations::fromScript;
}

void InputLayoutShapeLocations::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<InputLayoutShapeLocations>(s, path, lua, tenv);
}
}
