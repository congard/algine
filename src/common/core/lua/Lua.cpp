#include <algine/core/lua/Lua.h>
#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/lua/TulzLuaTypes.h>

#include "AlgineCore.h"

namespace algine {
void Lua::init() {
    m_lua = std::make_unique<sol::state>();
    m_lua->open_libraries();

    m_lua->set("algine_require", [this](std::string_view lib) {
        if (lib == "core") {
            registerUsertype(AlgineCore());
        } else if (lib == "glm") {
            GLMLuaTypes::registerLuaUsertype(this);
        } else if (lib == "tulz") {
            TulzLuaTypes::registerLuaUsertype(this);
        }
    });

    m_lua->script(R"(
local lua_require = require

require = function(name)
    local algineStrLen = string.len("algine")

    if string.sub(name, 1, algineStrLen) == "algine" then
        return algine_require(string.sub(name, algineStrLen + 2))
    else
        return lua_require(name)
    end
end
)");
}

bool Lua::isInitialized() const {
    return m_lua != nullptr;
}

const std::unique_ptr<sol::state>& Lua::state() const {
    return m_lua;
}
} // algine