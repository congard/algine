#ifndef ALGINE_MUTABLEVALUE_H
#define ALGINE_MUTABLEVALUE_H

namespace algine {
template<typename T>
class MutableValue {
public:
    inline MutableValue()
        : m_hasChanged() {}

    inline MutableValue(const T &value)
        : m_value(value), m_hasChanged(true) {}

    inline MutableValue(T &&value)
        : m_value(value), m_hasChanged(true) {}

    inline MutableValue& operator=(const T &value) {
        m_value = value;
        m_hasChanged = true;
        return *this;
    }

    inline MutableValue& operator=(T &&value) {
        m_value = value;
        m_hasChanged = true;
        return *this;
    }

    inline operator T&() {
        return get();
    }

    inline T& get() {
        m_hasChanged = false;
        return m_value;
    }

    inline const T& read() const {
        return m_value;
    }

    inline void setChanged(bool changed) {
        m_hasChanged = changed;
    }

    inline bool hasChanged() const {
        return m_hasChanged;
    }

private:
    T m_value;
    bool m_hasChanged;
};
}

#endif //ALGINE_MUTABLEVALUE_H
