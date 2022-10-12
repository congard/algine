#ifndef ALGINE_PTRVIEW_H
#define ALGINE_PTRVIEW_H

#include <memory>

namespace algine {
template<typename T>
class PtrView {
public:
    template<typename U>
    inline PtrView(PtrView<U> ptr)
        : m_ptr(ptr.get()) {}

    template<typename U>
    inline PtrView(const std::shared_ptr<U> &ptr)
        : m_ptr(ptr.get()) {}

    template<typename U>
    inline PtrView(const std::unique_ptr<U> &ptr)
        : m_ptr(ptr.get()) {}

    template<typename U>
    inline PtrView(U *ptr)
        : m_ptr(ptr) {}

    template<typename U>
    inline PtrView& operator=(const Ptr<U> &ptr) {
        m_ptr = ptr.get();
        return *this;
    }

    template<typename U>
    inline PtrView& operator=(U *ptr) {
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

#endif //ALGINE_PTRVIEW_H
