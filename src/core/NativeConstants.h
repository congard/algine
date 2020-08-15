#ifndef ALGINE_NATIVECONSTANTS_H
#define ALGINE_NATIVECONSTANTS_H

#include <tulz/macros.h>

namespace algine {
namespace NativeConstants {
enum_class(Types,
        Byte = 0x1400,
        UnsignedByte = 0x1401,
        Short = 0x1402,
        UnsignedShort = 0x1403,
        Int = 0x1404,
        UnsignedInt = 0x1405,
        Float = 0x1406,
        HalfFloat = 0x140B,
        Double = 0x140A)
}
}

#endif //ALGINE_NATIVECONSTANTS_H
