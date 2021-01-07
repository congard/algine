#include <algine/core/window/Cursor.h>

#include <stdexcept>

#include "Icon2GLFWimage.h"

namespace algine {
Cursor::Cursor() = default;

void cursorDeleter(GLFWcursor *cursor) {
    glfwDestroyCursor(cursor);
}

Cursor::Cursor(const Icon &icon, int xHot, int yHot) {
    auto image = getGLFWimageFromIcon(icon);
    m_cursor.reset(glfwCreateCursor(&image, xHot, yHot), &cursorDeleter);
}

Cursor Cursor::getDefaultCursor(Default type) {
    auto getGLFWType = [&]() {
        switch (type) {
            case Default::Arrow: return GLFW_ARROW_CURSOR;
            case Default::TextInput: return GLFW_IBEAM_CURSOR;
            case Default::Crosshair: return GLFW_CROSSHAIR_CURSOR;
            case Default::Hand: return GLFW_HAND_CURSOR;
            case Default::HorizontalResizeArrow: return GLFW_HRESIZE_CURSOR;
            case Default::VerticalResizeArrow: return GLFW_VRESIZE_CURSOR;
        }

        throw std::invalid_argument("Unknown cursor type " + std::to_string(static_cast<int>(type)));
    };

    Cursor cursor;
    cursor.m_cursor.reset(glfwCreateStandardCursor(getGLFWType()), &cursorDeleter);

    return cursor;
}
}
