#ifndef ALGINE_ASSERT_CAST_H
#define ALGINE_ASSERT_CAST_H

#include <cassert>

namespace algine {
/**
 * Performs `dynamic_cast` with nullptr assertion.
 * This function template allows casting a pointer
 * of type S to a pointer of type T using `dynamic_cast`.
 * It asserts that the output pointer is not nullptr
 * after performing the cast.
 * @tparam T The target type.
 * @tparam S The source type.
 * @param obj The pointer to be casted.
 * @return The casted pointer of type T.
 */
template<typename T, typename S>
inline auto assert_cast(S *obj) {
    auto result = dynamic_cast<T>(obj);
    assert(result != nullptr);
    return result;
}
}

#endif //ALGINE_ASSERT_CAST_H
