#ifndef ALGINE_BLENDDOF_H
#define ALGINE_BLENDDOF_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace BlendDOF {
    namespace Settings {
        constant(SkipInterpRangeCheck, "ALGINE_DOF_SKIP_RANGE_CHECK")
    }

    namespace Vars {
        constant(COCMap, "cocMap")
        constant(DOFSigmaDivider, "dofSigmaDivider")
    }
}
}
}

#undef constant

#endif //ALGINE_BLENDDOF_H
