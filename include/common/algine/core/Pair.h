#ifndef ALGINE_PAIR_H
#define ALGINE_PAIR_H

#include <algorithm>
#include <utility>

namespace algine {
template<typename U, typename V>
class Pair {
public:
    using first_type = U;
    using second_type = V;

    U first;
    V second;

public:
    Pair(): first(), second() {}
    Pair(const U &first, const V &second): first(first), second(second) {}

    Pair(const Pair &other): first(other.first), second(other.second) {} // copy ctor
    Pair(Pair &&other) noexcept: first(std::move(other.first)), second(std::move(other.second)) {} // move ctor

    Pair& operator=(const Pair &other) { // copy assignment
        first = other.first;
        second = other.second;
        return *this;
    }

    Pair& operator=(Pair &&other) noexcept { // move assignment
        std::swap(first, other.first);
        std::swap(second, other.second);
        return *this;
    }
};
}

#endif //ALGINE_PAIR_H
