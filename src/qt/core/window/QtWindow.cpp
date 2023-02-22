#include <algine/gl.h>

#include <algine/core/window/QtWindow.h>
#include <algine/core/Content.h>
#include <algine/core/Engine.h>

#include <QMouseEvent>
#include <QKeyEvent>

#include "core/input/MouseKeyConverter.h"
#include "core/input/KeyboardKeyConverter.h"

namespace algine {
constexpr static auto maxClickDistanceSquared = 16.0f * 16.0f;
constexpr static auto maxClickDeltaTime = 250L; // in ms

QtWindow::QtWindow(Context shareContext, QWindow *parent)
    : m_parentContext(shareContext),
      m_cursorPos(-1.0f, -1.0f),
      QOpenGLWindow(
          shareContext.isInitialized() ? static_cast<QOpenGLContext*>(shareContext.m_context) : QOpenGLContext::globalShareContext(),
          NoPartialUpdate, parent)
{
    m_selfDestroy = Engine::addOnDestroyListener([this]() {
        delete this;
    });
}

QtWindow::~QtWindow() {
    m_selfDestroy.unsubscribe();
}

void QtWindow::renderLoop(int delayMs) {
    QObject::connect(&m_renderLoopTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_renderLoopTimer.start(delayMs);
}

void QtWindow::renderLoop() {
    renderLoop(0);
}

void QtWindow::stopRenderLoop() {
    m_renderLoopTimer.stop();
}

glm::vec2 QtWindow::getCursorPos() const {
    return m_cursorPos;
}

bool QtWindow::isKeyPressed(KeyboardKey key) const {
    return m_keyboardKeys.find(key) != m_keyboardKeys.end();
}

bool QtWindow::isMouseKeyPressed(MouseKey key) const {
    return m_mouseKeys.find(getQtMouseButton(key)) != m_mouseKeys.end();
}

void QtWindow::setDimensions(int width, int height) {
    setMinimumSize({
        static_cast<int>(round(width / devicePixelRatio())),
        static_cast<int>(round(height / devicePixelRatio()))
    });
}

inline auto qEventHandler(EventHandler *eventHandler) {
    return static_cast<QtWindowEventHandler*>(eventHandler);
}

#define qEventHandlerCall(...) if (auto handler = qEventHandler(getEventHandler()); handler) handler->__VA_ARGS__

void QtWindow::setEventHandler(QtWindowEventHandler *eventHandler) {
    BaseWindow::setEventHandler(eventHandler);
}

Context QtWindow::getContext() {
    return {
        .m_context = context(),
        .m_surface = static_cast<QSurface*>(this)
    };
}

Context QtWindow::getParentContext() const {
    return m_parentContext;
}

glm::ivec2 QtWindow::getViewport() {
    qreal retinaScale = devicePixelRatio();
    return {
        width() * retinaScale,
        height() * retinaScale
    };
}

void QtWindow::addOnInitializedListener(const tulz::Observer &observer) {
    m_onInitialized.subscribe(observer);
}

void QtWindow::initializeGL() {
    if (auto code = glewInit(); code != GLEW_NO_ERROR) {
        throw std::runtime_error("GLEW init failed, error code " + std::to_string(code));
    }

    Engine::m_appContext = getContext();

    m_onInitialized.notify();
}

void QtWindow::resizeGL(int w, int h) {
    sizeChanged();
}

void QtWindow::paintGL() {
    m_content->render();
}

void QtWindow::mousePressEvent(QMouseEvent *event) {
    setCursorPos(event);

    auto button = event->button();

    m_mouseKeys[button] = {
        .pressTime = Engine::time(),
        .maxDeltaSquared = 0,
        .pressCoords = m_cursorPos
    };

    qEventHandlerCall(mouseKeyPress(getMouseKey(button), *this));
}

void QtWindow::mouseReleaseEvent(QMouseEvent *event) {
    Qt::MouseButton button = event->button();

    if (auto eventHandler = qEventHandler(getEventHandler()); eventHandler) {
        auto &info = m_mouseKeys[button];

        long releaseTime = Engine::time();
        long deltaTime = releaseTime - info.pressTime;

        if (info.maxDeltaSquared <= maxClickDistanceSquared && deltaTime <= maxClickDeltaTime) {
            eventHandler->mouseClick(getMouseKey(button), *this);
        }
    }

    m_mouseKeys.erase(button);
}

void QtWindow::mouseMoveEvent(QMouseEvent *event) {
    setCursorPos(event);

    for (auto & [button, info] : m_mouseKeys) {
        auto d = info.pressCoords - m_cursorPos;
        auto distanceSquared = powf(d.x, 2) + powf(d.y, 2);
        info.maxDeltaSquared = std::max(info.maxDeltaSquared, distanceSquared);
    }

    qEventHandlerCall(mouseMove(m_cursorPos.x, m_cursorPos.y, *this));
}

void QtWindow::keyPressEvent(QKeyEvent *event) {
    auto key = getKeyboardKey(event);

    if (event->isAutoRepeat()) {
        qEventHandlerCall(keyboardKeyRepeat(key, *this));
    } else {
        m_keyboardKeys.emplace(key);
        qEventHandlerCall(keyboardKeyPress(key, *this));
    }
}

void QtWindow::keyReleaseEvent(QKeyEvent *event) {
    auto key = getKeyboardKey(event);
    m_keyboardKeys.erase(key);
    qEventHandlerCall(keyboardKeyRelease(key, *this));
}

void QtWindow::exposeEvent(QExposeEvent *ev) {
    sizeChanged();
    QPaintDeviceWindow::exposeEvent(ev);
}

void QtWindow::focusInEvent(QFocusEvent *ev) {
    qEventHandlerCall(windowFocus(*this));
    QWindow::focusInEvent(ev);
}

void QtWindow::focusOutEvent(QFocusEvent *ev) {
    qEventHandlerCall(windowFocusLost(*this));
    QWindow::focusOutEvent(ev);
}

void QtWindow::hideEvent(QHideEvent *ev) {
    qEventHandlerCall(windowIconify(*this));
    QWindow::hideEvent(ev);
}

void QtWindow::showEvent(QShowEvent *ev) {
    qEventHandlerCall(windowRestore(*this));
    QWindow::showEvent(ev);
}

void QtWindow::moveEvent(QMoveEvent *ev) {
    auto pos = ev->pos() * devicePixelRatio();
    qEventHandlerCall(windowPosChange(pos.x(), pos.y(), *this));
    QWindow::moveEvent(ev);
}

void QtWindow::setCursorPos(QMouseEvent *event) {
    auto &localPos = event->localPos();
    m_cursorPos = glm::vec2(localPos.x(), localPos.y()) * static_cast<float>(devicePixelRatio());
}

void QtWindow::sizeChanged() {
    auto viewport = getViewport();

    if (auto &content = getContent(); content != nullptr) {
        content->m_width = viewport.x;
        content->m_height = viewport.y;
    }

    qEventHandlerCall(windowSizeChange(viewport.x, viewport.y, *this));
}
}
