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

    inline PtrView(std::nullptr_t)
        : m_ptr(nullptr) {}

    template<typename U>
    inline PtrView& operator=(const std::shared_ptr<U> &ptr) {
        m_ptr = ptr.get();
        return *this;
    }

    template<typename U>
    inline PtrView& operator=(U *ptr) {
        m_ptr = ptr;
        return *this;
    }

    template<typename U> inline bool operator==(PtrView<U> ptr) { return m_ptr == ptr.get(); }
    template<typename U> inline bool operator==(const std::shared_ptr<U> &ptr) { return m_ptr == ptr.get(); }
    template<typename U> inline bool operator==(const std::unique_ptr<U> &ptr) { return m_ptr == ptr.get(); }
    template<typename U> inline bool operator==(U *ptr) { return m_ptr == ptr; }
    inline bool operator==(std::nullptr_t) { return m_ptr == nullptr; }

    template<typename U> inline bool operator!=(PtrView<U> ptr) { return !(*this == ptr); }
    template<typename U> inline bool operator!=(const std::shared_ptr<U> &ptr) { return !(*this == ptr); }
    template<typename U> inline bool operator!=(const std::unique_ptr<U> &ptr) { return !(*this == ptr); }
    template<typename U> inline bool operator!=(U *ptr) { return !(*this == ptr); }
    inline bool operator!=(std::nullptr_t) { return m_ptr != nullptr; }

    inline T& operator*() {
        return *m_ptr;
    }

    inline T* operator->() const {
        return m_ptr;
    }

    inline T* get() const {
        return m_ptr;
    }

private:
    T *m_ptr;
};
}

#endif //ALGINE_PTRVIEW_H
