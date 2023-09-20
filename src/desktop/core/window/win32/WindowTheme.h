#if !defined(ALGINE_WINDOWTHEME_H) && defined(_WIN32)
#define ALGINE_WINDOWTHEME_H

struct GLFWwindow;

namespace algine::internal::win32::WindowTheme {
/**
 * Applies the system theme to the specified window.
 * @param window The GLFW window to apply the theme to.
 * @return `true` if the theme is successfully applied, `false` otherwise.
 */
bool applyTheme(GLFWwindow *window);
}

#endif //ALGINE_WINDOWTHEME_H
