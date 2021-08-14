#ifndef ALGINE_AUTORAWPTR_H
#define ALGINE_AUTORAWPTR_H

#include <algine/core/Ptr.h>

namespace algine {
template<typename T>
class AutoRawPtr {
public:
    template<typename U>
    AutoRawPtr(const Ptr<U> &ptr)
        : m_ptr(ptr.get()) {}

    template<typename U>
    AutoRawPtr(U *ptr)
        : m_ptr(ptr) {}

    template<typename U>
    AutoRawPtr& operator=(const Ptr<U> &ptr) {
        m_ptr = ptr.get();
        return *this;
    }

    template<typename U>
    AutoRawPtr& operator=(U *ptr) {
        m_ptr = ptr;
        return *this;
    }

    T* operator->() const {
        return m_ptr;
    }

    T* get() const {
        return m_ptr;
    }

private:
    T *m_ptr;
};
}

#endif //ALGINE_AUTORAWPTR_H
