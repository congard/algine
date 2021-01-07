#ifndef ALGINE_ICON2GLFWIMAGE_H
#define ALGINE_ICON2GLFWIMAGE_H

#include <algine/core/window/Icon.h>

#include <GLFW/glfw3.h>

namespace algine {
inline GLFWimage getGLFWimageFromIcon(const Icon &icon) {
    GLFWimage image;
    image.width = icon.getWidth();
    image.height = icon.getHeight();
    image.pixels = icon.getPixelData().get();

    return image;
}
}

#endif //ALGINE_ICON2GLFWIMAGE_H
