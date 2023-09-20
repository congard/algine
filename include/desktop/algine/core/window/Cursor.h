#ifndef ALGINE_CURSOR_H
#define ALGINE_CURSOR_H

#include <algine/core/window/Icon.h>

class GLFWcursor;

namespace algine {
class AL_EXPORT Cursor {
    friend class GLFWWindow;

public:
    enum class Default {
        Arrow,
        TextInput,
        Crosshair,
        Hand,
        HorizontalResizeArrow,
        VerticalResizeArrow
    };

public:
    Cursor();
    explicit Cursor(const Icon &icon, int xHot = 0, int yHot = 0);

    static Cursor getDefaultCursor(Default type);

private:
    Ptr<GLFWcursor> m_cursor;
};
}

#endif //ALGINE_CURSOR_H
