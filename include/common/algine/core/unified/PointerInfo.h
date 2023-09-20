#ifndef ALGINE_POINTERINFO_H
#define ALGINE_POINTERINFO_H

#include <algine/core/input/MouseKey.h>
#include <algine/platform.h>

#include <glm/vec2.hpp>
#include <variant>

namespace algine {
#ifdef ALGINE_QT_PLATFORM
class QtWindow;
#elif !defined(__ANDROID__)
class GLFWWindow;
#endif

class AL_EXPORT PointerInfo {
public:
    using Finger = int;
    using Pointer = std::variant<MouseKey, Finger>;

public:
#ifdef __ANDROID__
    inline PointerInfo(float x, float y)
        : m_pos(x, y) {}

    template<typename T>
    PointerInfo(float x, float y, T pointer)
        : m_pos(x, y), m_pointer(pointer) {}
#elif defined(ALGINE_QT_PLATFORM)
    inline PointerInfo(glm::vec2 pos, QtWindow &window)
        : m_pos(pos), m_window(window) {}

    template<typename T>
    PointerInfo(glm::vec2 pos, T pointer, QtWindow &window)
        : m_pos(pos), m_pointer(pointer), m_window(window) {}

    inline QtWindow& getWindow() { return m_window; }
#else
    inline PointerInfo(const glm::vec2 &pos, GLFWWindow &window)
        : m_pos(pos), m_window(window) {}

    template<typename T>
    PointerInfo(const glm::vec2 &pos, T pointer, GLFWWindow &window)
        : m_pos(pos), m_pointer(pointer), m_window(window) {}

    inline GLFWWindow& getWindow() { return m_window; }
#endif

    void setPos(const glm::vec2 &pos);
    const glm::vec2& getPos() const;

    void setX(float x);
    void setY(float y);
    float getX() const;
    float getY() const;

    void setMouseKey(MouseKey key);
    void setFinger(Finger id);
    bool isMouseKey() const;
    bool isFinger() const;
    MouseKey getMouseKey() const;
    Finger getFinger() const;
    const Pointer& getPointer() const;

    bool isPointerActive(Pointer pointer) const;
    glm::vec2 getPointerPos(Pointer pointer) const;

private:
    glm::vec2 m_pos;
    Pointer m_pointer;

#ifdef ALGINE_QT_PLATFORM
    QtWindow &m_window;
#elif !defined(__ANDROID__)
    GLFWWindow &m_window;
#endif
};

inline void PointerInfo::setPos(const glm::vec2 &pos) {
    m_pos = pos;
}

inline const glm::vec2& PointerInfo::getPos() const {
    return m_pos;
}

inline void PointerInfo::setX(float x) {
    m_pos.x = x;
}

inline void PointerInfo::setY(float y) {
    m_pos.y = y;
}

inline float PointerInfo::getX() const {
    return m_pos.x;
}

inline float PointerInfo::getY() const {
    return m_pos.y;
}

inline void PointerInfo::setMouseKey(MouseKey key) {
    m_pointer = key;
}

inline void PointerInfo::setFinger(Finger id) {
    m_pointer = id;
}

inline bool PointerInfo::isMouseKey() const {
    return std::get_if<MouseKey>(&m_pointer);
}

inline bool PointerInfo::isFinger() const {
    return std::get_if<Finger>(&m_pointer);
}

inline MouseKey PointerInfo::getMouseKey() const {
    return std::get<MouseKey>(m_pointer);
}

inline PointerInfo::Finger PointerInfo::getFinger() const {
    return std::get<Finger>(m_pointer);
}

inline const PointerInfo::Pointer& PointerInfo::getPointer() const {
    return m_pointer;
}
} // algine

#endif //ALGINE_POINTERINFO_H
