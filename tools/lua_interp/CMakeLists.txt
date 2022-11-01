cmake_minimum_required(VERSION 3.10)
project(algine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("../../deps/lua-src" "${CMAKE_CURRENT_BINARY_DIR}/deps/lua")
add_subdirectory("../../deps/sol2-src" "${CMAKE_CURRENT_BINARY_DIR}/deps/sol2")
add_executable(lua_interp lua_interp.cpp)
target_include_directories(lua_interp PRIVATE "../../deps/sol2-src/include" "../../deps/lua-src/lua-5.4.4/include")
target_link_libraries(lua_interp lua_static)
