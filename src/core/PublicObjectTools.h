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
}
}
