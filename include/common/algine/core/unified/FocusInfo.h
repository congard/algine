#ifndef ALGINE_FOCUSINFO_H
#define ALGINE_FOCUSINFO_H

#include <algine/platform.h>

namespace algine {
#ifdef ALGINE_QT_PLATFORM
class QtWindow;
#elif !defined(__ANDROID__)
class GLFWWindow;
#endif

class AL_EXPORT FocusInfo {
public:
    enum Reason {
        Unknown,
        Focused,
        FocusLost,
        WindowIconified,
        WindowRestored,
        AppPaused,
        AppResumed
    };

public:
#ifdef __ANDROID__
    inline FocusInfo(Reason reason)
        : m_reason(reason) {}
#elif defined(ALGINE_QT_PLATFORM)
    inline FocusInfo(Reason reason, QtWindow &window)
        : m_reason(reason), m_window(window) {}

    inline QtWindow& getWindow() { return m_window; }
#else
    inline FocusInfo(Reason reason, GLFWWindow &window)
        : m_reason(reason), m_window(window) {}

    inline GLFWWindow& getWindow() { return m_window; }
#endif

    void setReason(Reason reason);
    Reason getReason() const;

private:
    Reason m_reason {Reason::Unknown};

#ifdef ALGINE_QT_PLATFORM
    QtWindow &m_window;
#elif !defined(__ANDROID__)
    GLFWWindow &m_window;
#endif
};

inline void FocusInfo::setReason(Reason reason) {
    m_reason = reason;
}

inline FocusInfo::Reason FocusInfo::getReason() const {
    return m_reason;
}
}

#endif //ALGINE_FOCUSINFO_H
