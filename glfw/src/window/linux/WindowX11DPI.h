#if !defined(ALGINE_WINDOWX11DPI_H) && defined(__linux__)
#define ALGINE_WINDOWX11DPI_H

#include <cstdint>

struct GLFWwindow;

namespace algine::internal::linux_x11::WindowDPI {
/**
 * Returns the DPI value of the current display.
 * @note This function uses the display used by GLFW,
 * not the display used by a particular window
 * @return The DPI value.
 */
uint32_t getDPI();
} // algine::internal::linux_x11::WindowDPI

#endif //ALGINE_WINDOWX11DPI_H
