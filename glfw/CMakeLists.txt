set(ALGINE_GLFW_HEADERS_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include")
set(ALGINE_GLFW_HEADERS "${ALGINE_GLFW_HEADERS_INCLUDE}/algine")

target_include_directories(algine PUBLIC "${ALGINE_GLFW_HEADERS_INCLUDE}")

add_subdirectory(src/window)
add_subdirectory(src/context)