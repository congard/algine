#include <algine/core/window/GLFWWindow.h>
#include <algine/core/log/Log.h>
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

#ifdef _WIN32
    #include "win32/WindowTheme.h"
#endif

using namespace glm;

namespace algine {
constexpr static auto maxClickDistance = 16.0;
constexpr static int64_t maxClickDeltaTime = 250; // in ms

GLFWWindow::GLFWWindow(Context context)
    : GLFWWindow("Algine", 512, 512, context) {}

GLFWWindow::GLFWWindow(std::string title, uint width, uint height, Context context)
    : m_window(nullptr),
      m_pos(0),
      m_fullscreenDimensions(-1),
      m_cursorMode(CursorMode::Normal),
      m_mouseTracking(false),
      m_keyboardTracking(false),
      m_windowStateTracking(false),
      m_renderLoopRunning(false),
      m_viewport(),
      m_title(std::move(title)),
      m_dimensions(width, height),
      m_parentContext(context)
{
    create();
}

void GLFWWindow::create() {
    // additional calls to an already initialized library will return GLFW_TRUE immediately
    // https://www.glfw.org/docs/3.3/intro_guide.html#intro_init_init
    if (!glfwInit())
        throw std::runtime_error("GLFW init failed");

    if (Engine::getGraphicsAPI() != Engine::GraphicsAPI::Core)
        throw std::runtime_error("Invalid graphics API: only Core accepted");

    const auto apiVersion = Engine::getAPIVersion();

    if (apiVersion < 100 || apiVersion > 460)
        throw std::runtime_error("Invalid API version. Min is 100, max is 460. Your value: " + std::to_string(apiVersion));

    int majorVersion = apiVersion / 100;
    int minorVersion = (apiVersion - majorVersion * 100) / 10;

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, Log::isEnabled(Logger::Type::Debug));

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

    if (Log::isEnabled(Logger::Type::Debug)) {
        auto logger = Log::debug("GLFWWindow");
        logger << "Time: " << Engine::time() << "\n";
        logger << "GPU Vendor: " << Engine::getGPUVendor() << "\n";
        logger << "GPU Renderer: " << Engine::getGPURenderer() << "\n";

        enableDebugOutput();
    }

    if (!Engine::m_appContext.m_context) {
        Engine::m_appContext.m_context = m_window;
    }

#ifdef _WIN32
    internal::win32::WindowTheme::applyTheme(m_window);
#endif
}

void GLFWWindow::close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    stopRenderLoop();
}

void GLFWWindow::iconify() {
    glfwIconifyWindow(m_window);
}

void GLFWWindow::restore() {
    glfwRestoreWindow(m_window);
}

void GLFWWindow::maximize() {
    glfwMaximizeWindow(m_window);
}

void GLFWWindow::renderFrame() {
    if (m_content)
        m_content->render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();

    processTasks();
}

void GLFWWindow::renderLoop() {
    if (m_renderLoopRunning) {
        return;
    } else {
        m_renderLoopRunning = true;
    }

    while (isRenderLoopRunning()) {
        renderFrame();
    }
}

void GLFWWindow::stopRenderLoop() {
    m_renderLoopRunning = false;
}

dvec2 GLFWWindow::getCursorPos() const {
    dvec2 pos;

    glfwGetCursorPos(m_window, &pos.x, &pos.y);

    return pos;
}

bool GLFWWindow::isKeyPressed(KeyboardKey key) const {
    return glfwGetKey(m_window, getGLFWKeyValue(key)) == GLFW_PRESS;
}

bool GLFWWindow::isMouseKeyPressed(MouseKey key) const {
    return glfwGetMouseButton(m_window, getGLFWMouseKeyValue(key)) == GLFW_PRESS;
}

void GLFWWindow::setTitle(const std::string &title) {
    m_title = title;

    glfwSetWindowTitle(m_window, title.c_str());
}

void GLFWWindow::setIcon(const Icon &icon) {
    auto image = getGLFWimageFromIcon(icon);

    glfwSetWindowIcon(m_window, 1, &image);
}

void GLFWWindow::setIcon(const std::vector<Icon> &icons) {
    auto images = new GLFWimage[icons.size()];

    for (int i = 0; i < icons.size(); i++)
        images[i] = getGLFWimageFromIcon(icons[i]);

    glfwSetWindowIcon(m_window, icons.size(), images);

    delete[] images;
}

void GLFWWindow::setCursor(const Cursor &cursor) {
    m_cursor = cursor;

    glfwSetCursor(m_window, cursor.m_cursor.get());
}

void GLFWWindow::setDefaultCursor() {
    m_cursor.m_cursor = nullptr;

    glfwSetCursor(m_window, nullptr);
}

void GLFWWindow::setPos(int x, int y) {
    glfwSetWindowPos(m_window, x, y);
}

void GLFWWindow::setDimensions(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
}

void GLFWWindow::setFullscreenDimensions(int width, int height) {
    m_fullscreenDimensions.x = width;
    m_fullscreenDimensions.y = height;
}

void GLFWWindow::setCursorMode(CursorMode mode) {
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

void GLFWWindow::setOpacity(float opacity) {
    glfwSetWindowOpacity(m_window, opacity);
}

void GLFWWindow::setContent(Content *content, bool deleteOld) {
    if (content) {
        requestViewport();
    }

    Window::setContent(content, deleteOld);
}

void GLFWWindow::setEventHandler(WindowEventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

inline static auto WEH(EventHandler *eventHandler) {
    return static_cast<WindowEventHandler*>(eventHandler);
}

void GLFWWindow::setMouseTracking(bool tracking) {
    m_mouseTracking = tracking;

    if (tracking) {
        // mouse press/release/click tracking
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow *glfwWindow, int button, int action, int mods) {
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            auto eventHandler = WEH(window->getEventHandler());
            auto &mouseKeysInfo = window->m_mouseKeys;
            auto mouseKey = getMouseKey(button);

            if (action == GLFW_PRESS) {
                mouseKeysInfo[button] = GLFWWindow::MouseKeyInfo {Engine::time(), 0, window->getCursorPos()};

                // send press event
                if (eventHandler != nullptr) {
                    eventHandler->mouseKeyPress(mouseKey, *window);
                }
            } else if (action == GLFW_RELEASE) {
                if (eventHandler != nullptr) {
                    // send release event
                    eventHandler->mouseKeyRelease(mouseKey, *window);

                    // check click event conditions
                    auto releaseTime = Engine::time();
                    auto deltaTime = releaseTime - mouseKeysInfo[button].pressTime;

                    if (mouseKeysInfo[button].maxDelta <= maxClickDistance && deltaTime <= maxClickDeltaTime) {
                        eventHandler->mouseClick(mouseKey, *window);
                    }

                    mouseKeysInfo.erase(button);
                }
            } else {
                throw std::runtime_error("Mouse tracking - unknown action code: " + std::to_string(action));
            }
        });

        // mouse move tracking
        glfwSetCursorPosCallback(m_window, [](GLFWwindow *glfwWindow, double x, double y) {
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

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

void GLFWWindow::setKeyboardTracking(bool tracking) {
    m_keyboardTracking = tracking;

    if (tracking) {
        glfwSetKeyCallback(m_window, [](GLFWwindow *glfwWindow, int key, int scancode, int action, int mode) {
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

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

void GLFWWindow::setWindowStateTracking(bool tracking) {
    m_windowStateTracking = tracking;

    if (tracking) {
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow *glfwWindow, int width, int height) {
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

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
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            if (auto eventHandler = WEH(window->getEventHandler()); eventHandler != nullptr) {
                eventHandler->windowPosChange(x, y, *window);
            }
        });

        glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* glfwWindow, int iconified) {
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

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
            auto window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWindow));

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

void GLFWWindow::setFullscreen(bool fullscreen) {
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

void GLFWWindow::setResizable(bool resizable) {
    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, resizable);
}

void GLFWWindow::setDecorated(bool decorated) {
    glfwSetWindowAttrib(m_window, GLFW_DECORATED, decorated);
}

void GLFWWindow::setFloating(bool floating) {
    glfwSetWindowAttrib(m_window, GLFW_FLOATING, floating);
}

void GLFWWindow::setAutoIconify(bool autoIconify) {
    glfwSetWindowAttrib(m_window, GLFW_AUTO_ICONIFY, autoIconify);
}

void GLFWWindow::setFocusOnShow(bool focusOnShow) {
    glfwSetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW, focusOnShow);
}

const std::string& GLFWWindow::getTitle() const {
    return m_title;
}

const Cursor& GLFWWindow::getCursor() const {
    return m_cursor;
}

const ivec2& GLFWWindow::getPos() {
    requestPos();
    return m_pos;
}

const ivec2& GLFWWindow::getDimensions() {
    if (!isFullscreen()) {
        requestDimensions();

        return m_dimensions;
    } else {
        return m_fullscreenDimensions;
    }
}

const ivec2& GLFWWindow::getFullscreenDimensions() const {
    return m_fullscreenDimensions;
}

ivec2 GLFWWindow::getViewport() {
    requestViewport();
    return m_viewport;
}

GLFWWindow::CursorMode GLFWWindow::getCursorMode() const {
    return m_cursorMode;
}

Context GLFWWindow::getContext() const {
    return {m_window};
}

Context GLFWWindow::getParentContext() const {
    return m_parentContext;
}

float GLFWWindow::getOpacity() const {
    return glfwGetWindowOpacity(m_window);
}

bool GLFWWindow::isRenderLoopRunning() const {
    return m_renderLoopRunning && !glfwWindowShouldClose(m_window);
}

bool GLFWWindow::isMouseTracking() const {
    return m_mouseTracking;
}

bool GLFWWindow::isKeyboardTracking() const {
    return m_keyboardTracking;
}

bool GLFWWindow::isWindowStateTracking() const {
    return m_windowStateTracking;
}

bool GLFWWindow::isFullscreen() const {
    return glfwGetWindowMonitor(m_window) != nullptr;
}

bool GLFWWindow::isResizable() const {
    return glfwGetWindowAttrib(m_window, GLFW_RESIZABLE);
}

bool GLFWWindow::isDecorated() const {
    return glfwGetWindowAttrib(m_window, GLFW_DECORATED);
}

bool GLFWWindow::isFloating() const {
    return glfwGetWindowAttrib(m_window, GLFW_FLOATING);
}

bool GLFWWindow::isAutoIconify() const {
    return glfwGetWindowAttrib(m_window, GLFW_AUTO_ICONIFY);
}

bool GLFWWindow::isFocusOnShow() const {
    return glfwGetWindowAttrib(m_window, GLFW_FOCUS_ON_SHOW);
}

bool GLFWWindow::isIconified() const {
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
}

bool GLFWWindow::isMaximized() const {
    return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
}

void GLFWWindow::requestPos() {
    glfwGetWindowPos(m_window, &m_pos.x, &m_pos.y);
}

void GLFWWindow::requestDimensions() {
    glfwGetWindowSize(m_window, &m_dimensions.x, &m_dimensions.y);
}

void GLFWWindow::requestViewport() {
    glfwGetFramebufferSize(m_window, &m_viewport.x, &m_viewport.y);
}
}
