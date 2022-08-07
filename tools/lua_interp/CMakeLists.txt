cmake_minimum_required(VERSION 3.10)
project(algine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("../../libs/lua" "${CMAKE_CURRENT_BINARY_DIR}/libs/lua")
add_subdirectory("../../libs/sol2" "${CMAKE_CURRENT_BINARY_DIR}/libs/sol2")
add_executable(lua_interp lua_interp.cpp)
target_include_directories(lua_interp PRIVATE "../../libs/sol2/include" "../../libs/lua/lua-5.4.4/include")
target_link_libraries(lua_interp lua_static)
