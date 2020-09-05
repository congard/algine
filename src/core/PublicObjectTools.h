#include <algine/types.h>

#include <string>
#include <vector>

using namespace std;

namespace algine {
namespace PublicObjectTools {
constexpr uint notFound = static_cast<uint>(-1);

template<typename TPtr>
inline uint indexByName(const vector<TPtr> &array, const string &name) {
    for (uint i = 0; i < array.size(); i++) {
        if (array[i]->name == name) {
            return i;
        }
    }

    return notFound;
}

template<typename TPtr>
inline TPtr getByName(const vector<TPtr> &array, const string &name) {
    uint index = indexByName(array, name);

    if (index != notFound)
        return array[index];

    return nullptr;
}

template<typename TPtr>
inline typename TPtr::element_type* byName(const vector<TPtr> &array, const string &name) {
    uint index = indexByName(array, name);

    if (index != notFound)
        return array[index].get();

    return nullptr;
}

// below are the templates used in the managers for the getXXX function

template<typename TPtr, typename TMgr>
using CreatePtrFunction = TPtr (TMgr::*)();

template<typename TPtr, typename TMgr>
inline TPtr getPtr(TMgr *manager, CreatePtrFunction<TPtr, TMgr> createPtr) {
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

    return (manager->*createPtr)();
}
}
}
