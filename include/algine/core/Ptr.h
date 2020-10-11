#ifndef ALGINE_PTR_H
#define ALGINE_PTR_H

#include <memory>

namespace algine {
template<typename T>
using Ptr = std::shared_ptr<T>;
}

#endif //ALGINE_PTR_H
