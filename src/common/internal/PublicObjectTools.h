#ifndef ALGINE_PUBLICOBJECTTOOLS
#define ALGINE_PUBLICOBJECTTOOLS

#include <algine/core/PtrMaker.h>

#include <algine/types.h>

#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace algine::internal::PublicObjectTools {
constexpr uint notFound = static_cast<uint>(-1);

template<typename TPtr>
inline uint indexByName(const vector<TPtr> &array, string_view name) {
    for (uint i = 0; i < array.size(); i++) {
        if (array[i]->getName() == name) {
            return i;
        }
    }

    return notFound;
}

template<typename TPtr>
inline TPtr getByName(string_view name) {
    const auto &array = PtrMaker::PtrType<TPtr>::publicObjects;

    uint index = indexByName(array, name);

    if (index != notFound)
        return array[index];

    return nullptr;
}

template<typename T>
inline T* byName(string_view name) {
    const auto &array = T::publicObjects;

    uint index = indexByName(array, name);

    if (index != notFound)
        return array[index].get();

    return nullptr;
}

// below are the templates used in the managers for the get function

/**
 *
 * @tparam TPtr TPtr<T>
 * @tparam Types used for TMgr::create<Types...>();
 * @tparam TMgr
 * @param manager
 * @return
 */
template<typename TPtr, typename ...Types, typename TMgr>
inline TPtr getPtr(TMgr *manager) {
    using T = typename TPtr::element_type;

    const string &name = manager->getName();

    // if the object is public, it can be already created by something
    // if the object already created, just return it
    // otherwise just call create function

    if (manager->getAccess() == TMgr::Access::Public && !name.empty()) {
        auto obj = T::getByName(name);

        if (obj != nullptr) {
            return obj;
        }
    }

    if constexpr (sizeof...(Types) == 0) {
        return manager->create();
    } else {
        return manager->template create<Types...>();
    }
}

// below the function that needs to call in manager's
// create() after full Object creation

template<typename TPtr, typename TMgr>
inline void postCreateAccessOp(const string &typeName, const TMgr *manager, const TPtr &obj) {
    using Access = typename TMgr::Access;
    using T = typename TPtr::element_type;

    Access access = manager->getAccess();
    const string &name = manager->getName();

    if (access == Access::Public) {
        if (name.empty())
            throw runtime_error(typeName + " without name can't be public");

        if (T::byName(name) == nullptr) {
            T::publicObjects.emplace_back(obj);
        } else {
            throw runtime_error(typeName + " with the same name was already loaded");
        }
    }
}
}

#endif // ALGINE_PUBLICOBJECTTOOLS
