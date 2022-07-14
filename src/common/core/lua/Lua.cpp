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

    m_lua->set("algine_require", [this](std::string_view lib) {
        if (lib == "core") {
            registerUsertype(AlgineCore());
        } if (lib == "std") {
            registerUsertype(AlgineStd());
        } else if (lib == "glm") {
            GLMLuaTypes::registerLuaUsertype(this);
        } else if (lib == "tulz") {
            TulzLuaTypes::registerLuaUsertype(this);
        }
    });

    m_lua->script(core_lua);
}

bool Lua::isInitialized() const {
    return m_lua != nullptr;
}

const std::unique_ptr<sol::state>& Lua::state() const {
    return m_lua;
}
} // algine