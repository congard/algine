#ifndef ALGINE_PTRMAKER_H
#define ALGINE_PTRMAKER_H

#include <algine/core/Ptr.h>

/*
 * Can be used in 2 ways:
 *   Ptr<Foo> foo = PtrMaker::make();
 *   auto foo = PtrMaker::make<Foo>();
 */
class PtrMaker {
public:
    template<typename TPtr>
    using PtrType = typename TPtr::element_type;

    /*
     * to allow syntax like
     *   Ptr<Foo> foo = PtrMaker::make(Bar());
     * or (better, without extra move)
     *   Bar bar;
     *   Ptr<Foo> foo = PtrMaker::make(bar);
     * best for syntax like
     *   Ptr<Foo> foo = PtrMaker::make();
     */
    template<typename... Args>
    class AutoPtr {
    public:
        explicit constexpr AutoPtr(Args&&... args)
            : m_params(std::forward<Args>(args)...) {}

        template<typename TPtr>
        constexpr operator TPtr() {
            return invoke<TPtr>(std::index_sequence_for<Args...>());
        }

    private:
        template<typename TPtr, size_t... I>
        constexpr auto invoke(std::index_sequence<I...>) {
            return make<PtrType<TPtr>>(std::get<I>(m_params)...);
        }

    private:
        std::tuple<Args...> m_params;
    };

public:
    template<typename T = std::false_type, typename... Args>
    constexpr static auto make(Args&&... args) {
        if constexpr (std::is_same_v<T, std::false_type>) {
            return AutoPtr<Args...>(std::forward<Args>(args)...);
        } else {
            return make_ptr<T>(args...);
        }
    }

    template<typename TPtr>
    constexpr static void create(TPtr &ptr) {
        ptr = make<PtrType<TPtr>>();
    }

    template<typename TPtr, typename...U>
    constexpr static void create(TPtr &firstPtr, U&... pointers) {
        create(firstPtr);
        create(pointers...);
    }

private:
    template<typename T, typename... Args>
    constexpr static algine::Ptr<T> make_ptr(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

#endif //ALGINE_PTRMAKER_H
