#ifndef ALGINE_WINDOW_H
#define ALGINE_WINDOW_H

#include <algine/core/window/WindowEventHandler.h>
#include <algine/core/window/Cursor.h>
#include <algine/core/input/KeyboardKey.h>
#include <algine/core/input/MouseKey.h>
#include <algine/core/Window.h>
#include <algine/core/context/Context.h>
#include <algine/core/Ptr.h>
#include <algine/types.h>

#include <glm/vec2.hpp>

#include <string>
#include <vector>
#include <unordered_map>

struct GLFWwindow;

namespace algine {
class Content;

/**
 * Window implementation that uses GLFW as the backend.
 */
class AL_EXPORT GLFWWindow: public Window {
public:
    enum class CursorMode {
        Disabled,
        Hidden,
        Normal
    };

public:
    explicit GLFWWindow(Context context = {});
    GLFWWindow(std::string title, uint width, uint height, Context context = {});

    void close();
    void iconify();
    void restore();
    void maximize();

    void renderFrame();
    void renderLoop() override;

    void stopRenderLoop() override;

    glm::dvec2 getCursorPos() const;

    bool isKeyPressed(KeyboardKey key) const;
    bool isMouseKeyPressed(MouseKey key) const;

    void setTitle(const std::string &title);
    void setIcon(const Icon &icon);
    void setIcon(const std::vector<Icon> &icons);
    void setCursor(const Cursor &cursor);
    void setDefaultCursor();
    void setPos(int x, int y);
    void setDimensions(int width, int height);
    void setFullscreenDimensions(int width, int height);
    void setCursorMode(CursorMode mode);
    void setOpacity(float opacity);

    void setContent(Content *content, bool deleteOld) override;

    void setEventHandler(WindowEventHandler *eventHandler);

    void setMouseTracking(bool tracking);
    void setKeyboardTracking(bool tracking);
    void setWindowStateTracking(bool tracking);

    void setFullscreen(bool fullscreen);
    void setResizable(bool resizable);
    void setDecorated(bool decorated);
    void setFloating(bool floating);
    void setAutoIconify(bool autoIconify);
    void setFocusOnShow(bool focusOnShow);

    const std::string& getTitle() const;
    const Cursor& getCursor() const;
    const glm::ivec2& getPos();
    const glm::ivec2& getDimensions();
    const glm::ivec2& getFullscreenDimensions() const;
    glm::ivec2 getViewport() override;
    CursorMode getCursorMode() const;
    Context getContext() const;
    Context getParentContext() const;
    float getOpacity() const;

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

    using Window::setContent;

private:
    void create();

private:
    void requestPos();
    void requestDimensions();
    void requestViewport();

private:
    GLFWwindow *m_window;
    std::string m_title;
    Cursor m_cursor;
    glm::ivec2 m_pos, m_dimensions, m_fullscreenDimensions;
    CursorMode m_cursorMode;
    Context m_parentContext;
    bool m_mouseTracking, m_keyboardTracking, m_windowStateTracking;

private:
    bool m_renderLoopRunning;
    glm::ivec2 m_viewport;

private:
    struct MouseKeyInfo {
        int64_t pressTime;
        double maxDelta;
        glm::dvec2 pressCoords;
    };

    std::unordered_map<int, MouseKeyInfo> m_mouseKeys;
};
}

#endif //ALGINE_WINDOW_H
