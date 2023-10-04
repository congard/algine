#if !defined(ALGINE_EXAMPLES_WINDOWDPI_H) && defined(_WIN32)
#define ALGINE_EXAMPLES_WINDOWDPI_H

#include <cstdint>

struct GLFWwindow;

namespace algine::internal::win32::WindowDPI {
/**
 * Returns the DPI value of the specified window.
 * @param window The window to retrieve the DPI value from.
 * @return The DPI value.
 */
uint32_t getDPI(GLFWwindow *window);
} // namespace algine::internal::win32::WindowDPI

#endif//ALGINE_EXAMPLES_WINDOWDPI_H
