cmake_minimum_required(VERSION 3.18)
project(BasicWidgets)

set(CMAKE_CXX_STANDARD 20)

add_executable(BasicWidgets
        src/main.cpp
        src/MainContent.cpp
        src/MainContent.h
        src/ui/MainScene.cpp
        src/ui/MainScene.h
        src/ui/MainLayer.cpp
        src/ui/MainLayer.h)

target_link_libraries(BasicWidgets PRIVATE algine)

if (WIN32)
    algine_target_mklink(
            TARGET BasicWidgets
            ALGINE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/../algine"
            BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/../bin")
endif()
