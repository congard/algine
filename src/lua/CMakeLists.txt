set(ALGINE_LUA_HEADERS
        ${ALGINE_HEADERS}/lua/glm/GLMLuaTypes.h
        ${ALGINE_HEADERS}/lua/tulz/TulzLuaTypes.h

        ${ALGINE_HEADERS}/lua/Lua.h
        ${ALGINE_HEADERS}/lua/Module.h
        ${ALGINE_HEADERS}/lua/Scriptable.h
)

set(ALGINE_LUA_SOURCES
        glm/glm_string_cast.cpp glm/glm_string_cast.h
        glm/GLMLuaTypes.cpp

        tulz/TulzLuaTypes.cpp

        Lua.cpp
        Module.cpp
        Scriptable.cpp
)

target_sources(algine
        PUBLIC ${ALGINE_LUA_HEADERS}
        PRIVATE ${ALGINE_LUA_SOURCES}
)