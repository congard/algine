#include <algine/core/window/Window.h>

#include <algine/core/Content.h>
#include <algine/core/Engine.h>
#include <algine/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <stdexcept>
#include <utility>

#include "core/input/KeyboardKeyConverter.h"
#include "core/input/MouseKeyConverter.h"
#include "core/debug/Debug.h"
#include "Icon2GLFWimage.h"

using namespace std;
using namespace glm;

namespace algine {
constexpr static auto maxClickDistance = 16.0;
constexpr static auto maxClickDeltaTime = 250L; // in ms

#define initList \
    m_window(nullptr), \
    m_pos(0), \
    m_fullscreenDimensions(-1), \
    m_cursorMode(CursorMode::Normal), \
    m_mouseTracking(false), \
    m_keyboardTracking(false), \
    m_windowStateTracking(false), \
    m_renderLoopRunning(false)

Window::Window(Context context)
    : initList,
      m_title("Algine Window"),
      m_dimensions(512),
      m_parentContext(context)
{
    create();
}

Window::Window(string title, uint width, uint height, Context context)
    : initList,
      m_title(std::move(title)),
      m_dimensions(width, height),
      m_parentContext(context)
{
    create();
}

void Window::create() {
    // additional calls to an already initialized library will return GLFW_TRUE immediately
    // https://www.glfw.org/docs/3.3/intro_guide.html#intro_init_init
    if (!glfwInit())
        throw runtime_error("GLFW init failed");

    if (Engine::getGraphicsAPI() != Engine::GraphicsAPI::Core)
        throw runtime_error("Invalid graphics API: only Core accepted");

    const auto apiVersion = Engine::getAPIVersion();

    if (apiVersion < 100 || apiVersion > 460)
        throw runtime_error("Invalid API version. Min is 100, max is 460. Your value: " + to_string(apiVersion));

    int majorVersion = apiVersion / 100;
    int minorVersion = (apiVersion - majorVersion * 100) / 10;

    auto &debugWriter = Engine::getDebugWriter();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugWriter != nullptr);

    m_window = glfwCreateWindow(m_dimensions.x, m_dimensions.y, m_title.c_str(),
            nullptr, static_cast<GLFWwindow*>(m_parentContext.m_context));

    if (m_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Error initializing window");
    }

    glfwMakeContextCurrent(m_window);

    glfwSetWindowUserPointer(m_window, this);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    if (auto code = glewInit(); code != GLEW_NO_ERROR) {
        throw std::runtime_error("GLEW init failed, error code " + std::to_string(code));
    }

    if (debugWriter != nullptr) {
        auto logger = debugWriter->logger();
        logger << "Time: " << Engine::time() << "\n";
        logger << "GPU Vendor: " << Engine::getGPUVendor() << "\n";
        logger << "GPU Renderer: " << Engine::getGPURenderer() << "\n";

        enableDebugOutput();
    }

    if (!Engine::m_appContext.m_context) {
        Engine::m_appContext.m_context = m_window;
    }
}

void Window::close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    stopRenderLoop();
}

void Window::iconify() {
    glfwIconifyWindow(m_window);
}

void Window::restore() {
    glfwRestoreWindow(m_window);
}

void Window::maximize() {
    glfwMaximizeWindow(m_window);
}

void Window::renderFrame() {
    if (m_content)
        m_content->render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();

    processTasks();
}

void Window::renderLoop() {
    if (m_renderLoopRunning) {
        return;
    } else {
        m_renderLoopRunning = true;
    }

    while (isRenderLoopRunning()) {
        renderFrame();
    }
}

void Window::stopRenderLoop() {
    m_renderLoopRunning = false;
}

dvec2 Window::getCursorPos() const {
    dvec2 pos;

    glfwGetCursorPos(m_window, &pos.x, &pos.y);

    return pos;
}

bool Window::isKeyPressed(KeyboardKey key) const {
    return glfwGetKey(m_window, getGLFWKeyValue(key)) == GLFW_PRESS;
}

bool Window::isMouseKeyPressed(MouseKey key) const {
    return glfwGetMouseButton(m_window, getGLFWMouseKeyValue(key)) == GLFW_PRESS;
}

void Window::setTitle(const string &title) {
    m_title = title;

    glfwSetWindowTitle(m_window, title.c_str());
}

void Window::setIcon(const Icon &icon) {
    auto image = getGLFWimageFromIcon(icon);

    glfwSetWindowIcon(m_window, 1, &image);
}

void Window::setIcon(const vector<Icon> &icons) {
    auto images = new GLFWimage[icons.size()];

    for (int i = 0; i < icons.size(); i++)
        images[i] = getGLFWimageFromIcon(icons[i]);

    glfwSetWindowIcon(m_window, icons.size(), images);

    delete[] images;
}

void Window::setCursor(const Cursor &cursor) {
    m_cursor = cursor;

    glfwSetCursor(m_window, cursor.m_cursor.get());
}

void Window::setDefaultCursor() {
    m_cursor.m_cursor = nullptr;

    glfwSetCursor(m_window, nullptr);
}

void Window::setPos(int x, int y) {
    glfwSetWindowPos(m_window, x, y);
}

void Window::setDimensions(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
}

void Window::setFullscreenDimensions(int width, int height) {
    m_fullscreenDimensions.x = width;
    m_fullscreenDimensions.y = height;
}

void Window::setCursorMode(CursorMode mode) {
    m_cursorMode = mode;

    switch (mode) {
        case CursorMode::Disabled: {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        }
        case CursorMode::Hidden: {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        }
        case CursorMode::Normal: {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        }
    }
}

void Window::setOpacity(float opacity) {
    glfwSetWindowOpacity(m_window, opacity);
}

void Window::setContent(Content *content, bool deleteOld) {
    if (content) {
        requestViewport();
    }

    BaseWindow::setContent(content, deleteOld);
}

void Window::setEventHandler(WindowEventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

inline auto WEH(EventHandler *eventHandler) {
    return static_cast<WindowEventHandler*>(eventHandler);
}

void Window::setMouseTracking(bool tracking) {
    m_mouseTracking = tracking;

    if (tracking) {
        // mouse press/release/click tracking
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow *glfwWindow, int button, int action, int mods) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            auto eventHandler = WEH(window->getEventHandler());
            auto &mouseKeysInfo = window->m_mouseKeys;
            auto mouseKey = getMouseKey(button);

            if (action == GLFW_PRESS) {
                mouseKeysInfo[button] = Window::MouseKeyInfo {Engine::time(), 0, window->getCursorPos()};

                // send press event
                if (eventHandler != nullptr) {
                    eventHandler->mouseKeyPress(mouseKey, *window);
                }
            } else if (action == GLFW_RELEASE) {
                if (eventHandler != nullptr) {
                    // send release event
                    eventHandler->mouseKeyRelease(mouseKey, *window);

                    // check click event conditions
                    long releaseTime = Engine::time();
                    long deltaTime = releaseTime - mouseKeysInfo[button].pressTime;

                    if (mouseKeysInfo[button].maxDelta <= maxClickDistance && deltaTime <= maxClickDeltaTime) {
                        eventHandler->mouseClick(mouseKey, *window);
                    }

                    mouseKeysInfo.erase(button);
                }
            } else {
                throw runtime_error("Mouse tracking - unknown action code: " + to_string(action));
            }
        });

        // mouse move tracking
        glfwSetCursorPosCallback(m_window, [](GLFWwindow *glfwWindow, double x, double y) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            auto &mouseKeysInfo = window->m_mouseKeys;
            auto cursorPos = window->getCursorPos();

            // update max delta
            for (auto&& [button, info] : mouseKeysInfo) {
                auto delta = glm::distance(info.pressCoords, cursorPos);

                if (delta > info.maxDelta) {
                    info.maxDelta = delta;
                }
            }

            // send move event
            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                eventHandler->mouseMove(x, y, *window);
            }
        });
    } else {
        glfwSetMouseButtonCallback(m_window, nullptr);
        glfwSetCursorPosCallback(m_window, nullptr);
    }
}

void Window::setKeyboardTracking(bool tracking) {
    m_keyboardTracking = tracking;

    if (tracking) {
        glfwSetKeyCallback(m_window, [](GLFWwindow *glfwWindow, int key, int scancode, int action, int mode) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                switch (action) {
                    case GLFW_PRESS: {
                        eventHandler->keyboardKeyPress(getKeyboardKey(key), *window);
                        break;
                    }
                    case GLFW_REPEAT: {
                        eventHandler->keyboardKeyRepeat(getKeyboardKey(key), *window);
                        break;
                    }
                    case GLFW_RELEASE: {
                        eventHandler->keyboardKeyRelease(getKeyboardKey(key), *window);
                        break;
                    }
                    default: {

                    }
                }
            }
        });
    } else {
        glfwSetKeyCallback(m_window, nullptr);
    }
}

void Window::setWindowStateTracking(bool tracking) {
    m_windowStateTracking = tracking;

    if (tracking) {
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow *glfwWindow, int width, int height) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto content = window->getContent(); content != nullptr) {
                window->requestViewport();

                content->m_width = window->m_viewport.x;
                content->m_height = window->m_viewport.y;
            }

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                eventHandler->windowSizeChange(width, height, *window);
            }
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow *glfwWindow, int x, int y) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                eventHandler->windowPosChange(x, y, *window);
            }
        });

        glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* glfwWindow, int iconified) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                if (iconified) {
                    eventHandler->windowIconify(*window);
                } else {
                    eventHandler->windowRestore(*window);
                }
            }
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* glfwWindow, int focused) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                if (focused) {
                    eventHandler->windowFocus(*window);
                } else {
                    eventHandler->windowFocusLost(*window);
                }
            }
        });
    } else {
        glfwSetWindowSizeCallback(m_window, nullptr);
        glfwSetWindowPosCallback(m_window, nullptr);
        glfwSetWindowIconifyCallback(m_window, nullptr);
        glfwSetWindowFocusCallback(m_window, nullptr);
    }
}

void Window::setFullscreen(bool fullscreen) {
    if (isFullscreen() == fullscreen)
        return;

    if (fullscreen) {
        // backup window position and window size
        requestPos();
        requestDimensions();

        if (m_fullscreenDimensions == ivec2(-1)) {
            // get resolution of monitor
            auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            m_fullscreenDimensions.x = mode->width;
            m_fullscreenDimensions.y = mode->height;
        }

        // switch to full screen
        glfwSetWindowMonitor(
            m_window, glfwGetPrimaryMonitor(), 0, 0, m_fullscreenDimensions.x, m_fullscreenDimensions.y, 0
        );
    } else {
        // restore last window size and position
        glfwSetWindowMonitor(m_window, nullptr, m_pos.x, m_pos.y, m_dimensions.x, m_dimensions.y, 0);
    }
}

void Window::setResizable(bool resizable) {
    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, resizable);
}

void Window::setDecorated(bool decorated) {
    glfwSetWindowAttrib(m_window, GLFW_DECORATED, decorated);
}

void Window::setFloating(bool floating) {
    glfwSetWindowAttrib(m_window, GLFW_FLOATING, floating);
}

void Window::setAutoIconify(bool autoIconify) {
    glfwSetWindowAttrib(m_window, GLFW_AUTO_ICONIFY, autoIconify);
}

void Window::setFocusOnShow(bool focusOnShow) {
    glfwSetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW, focusOnShow);
}

const string& Window::getTitle() const {
    return m_title;
}

const Cursor& Window::getCursor() const {
    return m_cursor;
}

const ivec2& Window::getPos() {
    requestPos();
    return m_pos;
}

const ivec2& Window::getDimensions() {
    if (!isFullscreen()) {
        requestDimensions();

        return m_dimensions;
    } else {
        return m_fullscreenDimensions;
    }
}

const ivec2& Window::getFullscreenDimensions() const {
    return m_fullscreenDimensions;
}

ivec2 Window::getViewport() {
    requestViewport();
    return m_viewport;
}

Window::CursorMode Window::getCursorMode() const {
    return m_cursorMode;
}

Context Window::getContext() const {
    return {m_window};
}

Context Window::getParentContext() const {
    return m_parentContext;
}

float Window::getOpacity() const {
    return glfwGetWindowOpacity(m_window);
}

bool Window::isRenderLoopRunning() const {
    return m_renderLoopRunning && !glfwWindowShouldClose(m_window);
}

bool Window::isMouseTracking() const {
    return m_mouseTracking;
}

bool Window::isKeyboardTracking() const {
    return m_keyboardTracking;
}

bool Window::isWindowStateTracking() const {
    return m_windowStateTracking;
}

bool Window::isFullscreen() const {
    return glfwGetWindowMonitor(m_window) != nullptr;
}

bool Window::isResizable() const {
    return glfwGetWindowAttrib(m_window, GLFW_RESIZABLE);
}

bool Window::isDecorated() const {
    return glfwGetWindowAttrib(m_window, GLFW_DECORATED);
}

bool Window::isFloating() const {
    return glfwGetWindowAttrib(m_window, GLFW_FLOATING);
}

bool Window::isAutoIconify() const {
    return glfwGetWindowAttrib(m_window, GLFW_AUTO_ICONIFY);
}

bool Window::isFocusOnShow() const {
    return glfwGetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW);
}

bool Window::isIconified() const {
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
}

bool Window::isMaximized() const {
    return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
}

void Window::requestPos() {
    glfwGetWindowPos(m_window, &m_pos.x, &m_pos.y);
}

void Window::requestDimensions() {
    glfwGetWindowSize(m_window, &m_dimensions.x, &m_dimensions.y);
}

void Window::requestViewport() {
    glfwGetFramebufferSize(m_window, &m_viewport.x, &m_viewport.y);
}
}
