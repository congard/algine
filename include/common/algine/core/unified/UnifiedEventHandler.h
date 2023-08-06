#ifndef ALGINE_XEVENTHANDLER_H
#define ALGINE_XEVENTHANDLER_H

#ifdef __ANDROID__
    #include <algine/core/ScreenEventHandler.h>
    #define PlatformEventHandler ScreenEventHandler
#elif defined(ALGINE_QT_PLATFORM)
    #include <algine/core/window/QtWindowEventHandler.h>
    #define PlatformEventHandler QtWindowEventHandler
#else
    #include <algine/core/window/WindowEventHandler.h>
    #define PlatformEventHandler WindowEventHandler
#endif

#include <algine/core/input/MouseKey.h>
#include <algine/core/input/KeyboardKey.h>
#include <algine/core/unified/PointerInfo.h>
#include <algine/core/unified/FocusInfo.h>

#include <glm/vec2.hpp>

#include <forward_list>
#include <functional>

namespace algine {
class Event;

/**
 * Platform independent EventHandler
 */
class UnifiedEventHandler: public PlatformEventHandler {
public:
    using EventListener = std::function<void(const Event&)>;

public:
    virtual void onPointerMove(PointerInfo info);
    virtual void onPointerClick(PointerInfo info);
    virtual void onPointerPress(PointerInfo info);
    virtual void onPointerRelease(PointerInfo info);

    virtual void onKeyboardKeyPress(KeyboardKey key);
    virtual void onKeyboardKeyRelease(KeyboardKey key);
    virtual void onKeyboardKeyRepeat(KeyboardKey key);

    virtual void onSizeChanged(int width, int height);

    virtual void onFocusChanged(FocusInfo info);

public:
    /**
     * Handles such events as
     * <ul>
     *   <li><code>Event::Press</code></li>
     *   <li><code>Event::Release</code></li>
     *   <li><code>Event::Click</code></li>
     *   <li><code>Event::Move</code></li>
     * </ul>
     * @param listener
     * @see onPointerMove, onPointerClick, onPointerPress, onPointerRelease
     */
    void addPointerEventListener(const EventListener &listener);

    /**
     * Handles such events as
     * <ul>
     *   <li><code>Event::KeyPress</code></li>
     *   <li><code>Event::KeyRelease</code></li>
     *   <li><code>Event::KeyRepeat</code></li>
     * </ul>
     * @param listener
     * @see onKeyboardKeyPress, onKeyboardKeyRelease, onKeyboardKeyRepeat
     */
    void addKeyboardEventListener(const EventListener &listener);

    /**
     * Handles such events as
     * <ul>
     *   <li><code>Event::SurfaceSizeChange</code></li>
     *   <li><code>Event::SurfaceFocusChange</code></li>
     * </ul>
     * @param listener
     * @see onSizeChanged, onFocusChanged
     */
    void addSurfaceEventListener(const EventListener &listener);

    void clearPointerEventListeners();
    void clearKeyboardEventListeners();
    void clearSurfaceEventListeners();

protected:
#ifdef __ANDROID__
    void pointerDown(float x, float y, int pointerId) override;
    void pointerMove(float x, float y, int pointerId) override;
    void pointerUp(float x, float y, int pointerId) override;
    void pointerClick(float x, float y, int pointerId) override;

    void surfaceResized(int width, int height) override;

    void onPause() override;
    void onResume() override;
#elif defined(ALGINE_QT_PLATFORM)
    void mouseMove(float x, float y, QtWindow &window) override;
    void mouseClick(MouseKey key, QtWindow &window) override;
    void mouseKeyPress(MouseKey key, QtWindow &window) override;
    void mouseKeyRelease(MouseKey key, QtWindow &window) override;

    void keyboardKeyPress(KeyboardKey key, QtWindow &window) override;
    void keyboardKeyRelease(KeyboardKey key, QtWindow &window) override;
    void keyboardKeyRepeat(KeyboardKey key, QtWindow &window) override;

    void windowSizeChange(int width, int height, QtWindow &window) override;
    void windowIconify(QtWindow &window) override;
    void windowRestore(QtWindow &window) override;
    void windowFocusLost(QtWindow &window) override;
    void windowFocus(QtWindow &window) override;
#else
    void mouseMove(double x, double y, GLFWWindow &window) override;
    void mouseClick(MouseKey key, GLFWWindow &window) override;
    void mouseKeyPress(MouseKey key, GLFWWindow &window) override;
    void mouseKeyRelease(MouseKey key, GLFWWindow &window) override;

    void keyboardKeyPress(KeyboardKey key, GLFWWindow &window) override;
    void keyboardKeyRelease(KeyboardKey key, GLFWWindow &window) override;
    void keyboardKeyRepeat(KeyboardKey key, GLFWWindow &window) override;

    void windowSizeChange(int width, int height, GLFWWindow &window) override;
    void windowIconify(GLFWWindow &window) override;
    void windowRestore(GLFWWindow &window) override;
    void windowFocusLost(GLFWWindow &window) override;
    void windowFocus(GLFWWindow &window) override;
#endif

private:
    std::forward_list<EventListener> m_pointerListeners;
    std::forward_list<EventListener> m_keyboardListeners;
    std::forward_list<EventListener> m_surfaceListeners;
};
}

#undef PlatformEventHandler

#endif //ALGINE_XEVENTHANDLER_H
