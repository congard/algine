#ifndef ALGINE_WINDOW_H
#define ALGINE_WINDOW_H

#include <algine/core/window/DebugWriter.h>
#include <algine/core/window/Cursor.h>
#include <algine/core/input/KeyboardKey.h>
#include <algine/core/input/MouseKey.h>
#include <algine/core/Ptr.h>
#include <algine/types.h>

#include <glm/vec2.hpp>

#include <string>
#include <vector>
#include <unordered_map>

class GLFWwindow;

namespace algine {
class Content;

class Window {
public:
    enum class CursorMode {
        Disabled,
        Hidden,
        Normal
    };

public:
    Window();
    Window(std::string title, uint width, uint height);

    void create();
    void close();
    void iconify();
    void restore();
    void maximize();

    void renderFrame();
    void renderLoop();

    void stopRenderLoop();

    glm::dvec2 getCursorPos() const;

    bool isKeyPressed(KeyboardKey key) const;
    bool isMouseKeyPressed(MouseKey key) const;

    void setDebug(DebugWriter *debugWriter);

    void setTitle(const std::string &title);
    void setIcon(const Icon &icon);
    void setIcon(const std::vector<Icon> &icons);
    void setCursor(const Cursor &cursor);
    void setDefaultCursor();
    void setPos(int x, int y);
    void setDimensions(uint width, uint height);
    void setFullscreenDimensions(uint width, uint height);
    void setCursorMode(CursorMode mode);
    void setOpacity(float opacity);

    void setContent(const Ptr<Content> &content);
    void setContent(Content *content);

    void setMouseTracking(bool tracking);
    void setKeyboardTracking(bool tracking);
    void setWindowStateTracking(bool tracking);

    void setFullscreen(bool fullscreen);
    void setResizable(bool resizable);
    void setDecorated(bool decorated);
    void setFloating(bool floating);
    void setAutoIconify(bool autoIconify);
    void setFocusOnShow(bool focusOnShow);

    bool isDebug() const;

    const std::string& getTitle() const;
    const Cursor& getCursor() const;
    const glm::ivec2& getPos();
    const glm::ivec2& getDimensions();
    const glm::ivec2& getFullscreenDimensions() const;
    const glm::ivec2& getViewport();
    CursorMode getCursorMode() const;
    float getOpacity() const;

    const Ptr<Content>& getContent() const;

    bool isRenderLoopRunning() const;

    bool isMouseTracking() const;
    bool isKeyboardTracking() const;
    bool isWindowStateTracking() const;

    bool isFullscreen() const;
    bool isResizable() const;
    bool isDecorated() const;
    bool isFloating() const;
    bool isAutoIconify() const;
    bool isFocusOnShow() const;

    bool isIconified() const;
    bool isMaximized() const;

private:
    void requestPos();
    void requestDimensions();
    void requestViewport();

private:
    GLFWwindow *m_window;
    std::unique_ptr<DebugWriter> m_debugWriter;
    std::string m_title;
    Cursor m_cursor;
    glm::ivec2 m_pos, m_dimensions, m_fullscreenDimensions, m_viewport;
    CursorMode m_cursorMode;
    Ptr<Content> m_content;
    bool m_mouseTracking, m_keyboardTracking, m_windowStateTracking;

private:
    bool m_renderLoopRunning;

private:
    struct MouseKeyInfo {
        long pressTime;
        double maxDelta;
        glm::dvec2 pressCoords;
    };

    std::unordered_map<int, MouseKeyInfo> m_mouseKeys;
};
}

#endif //ALGINE_WINDOW_H
