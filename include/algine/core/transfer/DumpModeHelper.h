#ifndef ALGINE_DUMPMODEHELPER_H
#define ALGINE_DUMPMODEHELPER_H

#include <algine/types.h>

namespace algine {
template<typename T>
class DumpModeHelper {
public:
    enum DumpMode {
        Path,
        Dump
    };

public:
    // do not mark explicit because we really need implicit conversion
    // manager.setXXX({foo, bar});
    // instead of ugly
    // manager.setXXX({Manager::DumpModeHelper<T>(foo), ... });
    // just ignore Clang-Tidy warning
    DumpModeHelper(const T &obj, uint dumpMode = Dump)
        : dumpMode(dumpMode)
    {
        object = obj;
    }

public:
    T object;
    uint dumpMode;
};
}


#endif //ALGINE_DUMPMODEHELPER_H
