set(ALGINE_GLFW_WINDOW_HEADERS
        ${ALGINE_GLFW_HEADERS}/window/Cursor.h
        ${ALGINE_GLFW_HEADERS}/window/GLFWWindow.h
        ${ALGINE_GLFW_HEADERS}/window/Icon.h
        ${ALGINE_GLFW_HEADERS}/window/WindowEventHandler.h
)

set(ALGINE_GLFW_WINDOW_SOURCES
        input/KeyboardKeyConverter.h
        input/MouseKeyConverter.h

        unified/PointerInfo.cpp
        unified/UnifiedEventHandler.cpp

        Cursor.cpp
        GLFWWindow.cpp
        Icon.cpp
        Icon2GLFWimage.h
        WindowEventHandler.cpp
)

if (UNIX)
    list(APPEND ALGINE_GLFW_WINDOW_SOURCES
            linux/WindowX11DPI.cpp linux/WindowX11DPI.h)
elseif (WIN32)
    list(APPEND ALGINE_GLFW_WINDOW_SOURCES
            win32/WindowDPI.cpp win32/WindowDPI.h
            win32/WindowTheme.cpp win32/WindowTheme.h)
endif()

target_sources(algine
        PUBLIC ${ALGINE_GLFW_WINDOW_HEADERS}
        PRIVATE ${ALGINE_GLFW_WINDOW_SOURCES}
)