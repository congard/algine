#include <algine/core/window/Window.h>

#include <algine/core/window/Content.h>

#include <algine/core/Engine.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <stdexcept>
#include <utility>

#include "core/input/KeyboardKeyConverter.h"
#include "core/input/MouseKeyConverter.h"
#include "Icon2GLFWimage.h"

using namespace std;
using namespace glm;

namespace algine {
constexpr static auto maxClickDistance = 16.0; // TODO: replace 'constexpr static auto' with 'constant'
constexpr static auto maxClickDeltaTime = 250L; // in ms

#define initList \
    m_window(nullptr), \
    m_debugWriter(nullptr), \
    m_viewport(), \
    m_pos(0), \
    m_fullscreenDimensions(-1), \
    m_cursorMode(CursorMode::Normal), \
    m_content(nullptr), \
    m_mouseTracking(false), \
    m_keyboardTracking(false), \
    m_windowStateTracking(false), \
    m_renderLoopRunning(false)

Window::Window()
    : initList,
      m_title("Algine Window"),
      m_dimensions(512) {}

Window::Window(string title, uint width, uint height)
    : initList,
      m_title(std::move(title)),
      m_dimensions(width, height) {}

// taken from https://vallentin.dev/2015/02/23/debugging-opengl
void debugMessagesHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                          const void *userParam)
{
    auto writer = static_cast<DebugWriter*>(const_cast<void*>(userParam));
    auto &stream = writer->stream();

    // Some debug messages are just annoying informational messages
    if (id == 131185) // glBufferData
        return;

    stream << "Message: " << message << "\n";
    stream << "Source: ";

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            stream << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            stream << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            stream << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            stream << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            stream << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            stream << "Other";
            break;
        default:
            stream << "Unknown source " << source << "\n";
            break;
    }

    stream << "\n";
    stream << "Type: ";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            stream << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            stream << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            stream << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            stream << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            stream << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            stream << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            stream << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            stream << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            stream << "Other";
            break;
        default:
            stream << "Unknown type " << type << "\n";
            break;
    }

    stream << "\n";
    stream << "ID: " << id << "\n";
    stream << "Severity: ";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            stream << "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            stream << "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            stream << "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            stream << "Notification";
            break;
        default:
            stream << "Unknown severity " << severity << "\n";
            break;
    }

    stream << "\n\n";
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

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, m_debugWriter != nullptr);

    m_window = glfwCreateWindow(m_dimensions.x, m_dimensions.y, m_title.c_str(), nullptr, nullptr);

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

    if (m_debugWriter != nullptr) {
        auto &stream = m_debugWriter->stream();
        stream << "Time: " << Engine::time() << "\n";
        stream << "GPU Vendor: " << Engine::getGPUVendor() << "\n";
        stream << "GPU Renderer: " << Engine::getGPURenderer() << "\n\n";

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugMessagesHandler, m_debugWriter.get());
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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
    m_content->render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
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

void Window::setDebug(DebugWriter *debugWriter) {
    m_debugWriter.reset(debugWriter);
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

void Window::setDimensions(uint width, uint height) {
    glfwSetWindowSize(m_window, width, height);
}

void Window::setFullscreenDimensions(uint width, uint height) {
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

void Window::setContent(const Ptr<Content> &content) {
    if (m_content)
        m_content->m_window = nullptr;

    m_content = content;

    if (!m_content) {
        stopRenderLoop();
    } else {
        m_content->m_window = this;

        requestViewport();

        m_content->m_width = m_viewport.x;
        m_content->m_height = m_viewport.y;

        if (!m_content->isInitialized()) {
            m_content->init();
            m_content->m_isInitialized = true;
        }
    }
}

void Window::setContent(Content *content) {
    setContent(Ptr<Content>(content));
}

void Window::setEventHandler(WindowEventHandler *eventHandler) {
    m_eventHandler = eventHandler;
}

inline WindowEventHandler* getEventHandlerIfPresent(GLFWwindow *glfwWindow) {
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

    if (window) {
        return window->getEventHandler();
    }

    return nullptr;
}

void Window::setMouseTracking(bool tracking) {
    m_mouseTracking = tracking;

    if (tracking) {
        // mouse press/release/click tracking
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow *glfwWindow, int button, int action, int mods) {
            auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

            if (window == nullptr)
                return;

            auto eventHandler = window->getEventHandler();
            auto &mouseKeysInfo = window->m_mouseKeys;
            auto mouseKey = getMouseKey(button);

            if (action == GLFW_PRESS) {
                mouseKeysInfo[button] = Window::MouseKeyInfo {Engine::time(), 0, window->getCursorPos()};

                // send press event
                if (eventHandler != nullptr) {
                    eventHandler->mouseKeyPress(mouseKey);
                }
            } else if (action == GLFW_RELEASE) {
                if (eventHandler != nullptr) {
                    // send release event
                    eventHandler->mouseKeyRelease(mouseKey);

                    // check click event conditions
                    long releaseTime = Engine::time();
                    long deltaTime = releaseTime - mouseKeysInfo[button].pressTime;

                    if (mouseKeysInfo[button].maxDelta <= maxClickDistance && deltaTime <= maxClickDeltaTime) {
                        eventHandler->mouseClick(mouseKey);
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
            if (auto eventHandler = window->getEventHandler(); eventHandler != nullptr) {
                eventHandler->mouseMove(x, y);
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
        glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scancode, int action, int mode) {
            if (auto eventHandler = getEventHandlerIfPresent(window); eventHandler != nullptr) {
                switch (action) {
                    case GLFW_PRESS: {
                        eventHandler->keyboardKeyPress(getKeyboardKey(key));
                        break;
                    }
                    case GLFW_REPEAT: {
                        eventHandler->keyboardKeyRepeat(getKeyboardKey(key));
                        break;
                    }
                    case GLFW_RELEASE: {
                        eventHandler->keyboardKeyRelease(getKeyboardKey(key));
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
            if (auto eventHandler = getEventHandlerIfPresent(glfwWindow); eventHandler != nullptr) {
                eventHandler->windowSizeChange(width, height);
            }

            if (auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow)); window != nullptr) {
                if (auto &content = window->getContent(); content != nullptr) {
                    window->requestViewport();

                    content->m_width = window->m_viewport.x;
                    content->m_height = window->m_viewport.y;
                }
            }
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow *window, int x, int y) {
            if (auto eventHandler = getEventHandlerIfPresent(window); eventHandler != nullptr) {
                eventHandler->windowPosChange(x, y);
            }
        });
    } else {
        glfwSetWindowSizeCallback(m_window, nullptr);
        glfwSetWindowPosCallback(m_window, nullptr);
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
        glfwSetWindowMonitor(m_window, nullptr,  m_pos.x, m_pos.y, m_dimensions.x, m_dimensions.y, 0);
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

bool Window::isDebug() const {
    return m_debugWriter != nullptr;
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

const ivec2& Window::getViewport() {
    requestViewport();

    return m_viewport;
}

Window::CursorMode Window::getCursorMode() const {
    return m_cursorMode;
}

float Window::getOpacity() const {
    return glfwGetWindowOpacity(m_window);
}

const Ptr<Content>& Window::getContent() const {
    return m_content;
}

WindowEventHandler* Window::getEventHandler() const {
    return m_eventHandler;
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
