#ifndef ALGINE_NORMALMAPPING_H
#define ALGINE_NORMALMAPPING_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace NormalMapping {
    namespace Settings {
        constant(FromMap, "ALGINE_NORMAL_MAPPING_FROM_MAP")
        constant(Dual, "ALGINE_NORMAL_MAPPING_DUAL")
    }

    namespace Vars {
        constant(Switcher, "u_NormalMapping")
    }
}
}
}

#undef constant

#endif //ALGINE_NORMALMAPPING_H
