#ifndef ALGINE_QTWINDOW_H
#define ALGINE_QTWINDOW_H

#include <algine/gl.h>

#include <algine/core/Window.h>
#include <algine/core/context/Context.h>
#include <algine/core/window/QtWindowEventHandler.h>

#include <tulz/observer/Subject.h>

#include <unordered_map>
#include <unordered_set>

#include <QOpenGLWindow>
#include <QTimer>

namespace algine {
/**
 * Window implementation that uses QOpenGLWindow as the backend.
 * @note to use this class Qt must be installed (Qt5+) and
 * the Qt backend should be selected.
 */
class AL_EXPORT QtWindow: public Window, public QOpenGLWindow {
public:
    explicit QtWindow(Context shareContext = {}, QWindow *parent = nullptr);
    ~QtWindow() override;

    void renderLoop(int delayMs);
    void renderLoop() override;
    void stopRenderLoop() override;

    glm::vec2 getCursorPos() const;

    bool isKeyPressed(KeyboardKey key) const;
    bool isMouseKeyPressed(MouseKey key) const;

    void setDimensions(int width, int height);

    void setEventHandler(QtWindowEventHandler *eventHandler);

    Context getContext();
    Context getParentContext() const;

    glm::ivec2 getViewport() override;

    /**
     * @note The observer will be called from the rendering thread.
     * @param observer
     */
    tulz::Subscription<> addOnInitializedListener(const tulz::Observer<> &observer);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void exposeEvent(QExposeEvent *ev) override;
    void focusInEvent(QFocusEvent *ev) override;
    void focusOutEvent(QFocusEvent *ev) override;
    void hideEvent(QHideEvent *ev) override;
    void showEvent(QShowEvent *ev) override;
    void moveEvent(QMoveEvent *ev) override;

private:
    void setCursorPos(QMouseEvent *event);
    void sizeChanged();

private:
    Context m_parentContext;
    tulz::Subject<> m_onInitialized;

    QTimer m_renderLoopTimer;

private:
    struct MouseKeyInfo {
        long pressTime;
        float maxDeltaSquared;
        glm::vec2 pressCoords;
    };

    std::unordered_map<Qt::MouseButton, MouseKeyInfo> m_mouseKeys;
    std::unordered_set<KeyboardKey> m_keyboardKeys;

    glm::vec2 m_cursorPos;
};
}

#endif //ALGINE_QTWINDOW_H
