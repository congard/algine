#ifdef _WIN32

#include "WindowDPI.h"

#include <algine/core/log/Log.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#pragma comment(lib, "User32.lib")

#define LOG_TAG "win32::WindowDPI"

namespace algine::internal::win32::WindowDPI {
uint32_t getDPI(GLFWwindow *window) {
    auto handle = glfwGetWin32Window(window);
    auto dpi = GetDpiForWindow(handle);
    Log::debug(LOG_TAG) << "DPI: " << dpi;
    return dpi;
}
} // namespace algine::internal::win32::WindowDPI

#endif