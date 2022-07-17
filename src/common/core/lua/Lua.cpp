#include <algine/core/lua/Lua.h>
#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/lua/TulzLuaTypes.h>

#include "AlgineCore.h"
#include "AlgineStd.h"
#include "CoreLua.h"

namespace algine {
void Lua::init() {
    m_lua = std::make_unique<sol::state>();
    m_lua->open_libraries();
    initEnvironment(getGlobalEnvironment());
}

bool Lua::isInitialized() const {
    return m_lua != nullptr;
}

const std::unique_ptr<sol::state>& Lua::state() const {
    return m_lua;
}

sol::global_table Lua::createEnvironment() {
    sol::environment env(*m_lua, sol::create);
    sol::global_table tenv = env;
    initEnvironment(tenv);
    return tenv;
}

sol::global_table Lua::createEnvironment(const sol::global_table &parent) {
    return sol::environment(*m_lua, sol::create, sol::environment(parent));
}

sol::global_table& Lua::getGlobalEnvironment() const {
    return m_lua->globals();
}

void Lua::initEnvironment(sol::global_table &env) {
    env["algine_require"] = [this](std::string_view lib, sol::this_environment tenv) {
        sol::global_table env = *tenv.env;

        if (lib == "core") {
            registerUsertype(&env, AlgineCore());
        } if (lib == "std") {
            registerUsertype(&env, AlgineStd());
        } else if (lib == "glm") {
            GLMLuaTypes::registerLuaUsertype(this, &env);
        } else if (lib == "tulz") {
            TulzLuaTypes::registerLuaUsertype(this, &env);
        }
    };

    m_lua->script(core_lua, sol::environment(env));
}

sol::global_table& Lua::getEnv(Lua *lua, sol::global_table *env) {
    if (env)
        return *env;

    if (!lua)
        return Engine::getLua().state()->globals();

    if (!lua->isInitialized())
        lua->init();

    return lua->state()->globals();
}

bool Lua::isRegistered(sol::global_table &env, std::string_view type) {
    return env[type].valid();
}
} // algine