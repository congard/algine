#ifndef ALGINE_VARIANT_H
#define ALGINE_VARIANT_H

#include <variant>

namespace algine {
template<typename ...Args>
class Variant {
public:
    using variant = std::variant<Args...>;

public:
    Variant() = default;

    template<typename T>
    Variant(T &&value)
        : m_var(std::forward<T>(value)) {}

    template<typename T>
    Variant& operator=(T &&rhs) {
        m_var = std::forward<T>(rhs);
        return *this;
    }

    template<typename T>
    constexpr T& as() {
        return std::get<T>(m_var);
    }

    template<typename T>
    constexpr bool is() const {
        return std::get_if<T>(&m_var);
    }

    const variant& get_std() const {
        return m_var;
    }

private:
    variant m_var;
};
}

#endif //ALGINE_VARIANT_H
