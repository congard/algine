#ifndef ALGINE_BONESYSTEM_CONSTANTS_H
#define ALGINE_BONESYSTEM_CONSTANTS_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace BoneSystem {
    namespace Settings {
        constant(MaxBoneAttribsPerVertex, "MAX_BONE_ATTRIBS_PER_VERTEX")
        constant(MaxBones, "MAX_BONES")
    }

    namespace Vars {
        constant(InBoneIds, "inBoneIds[0]") // integer
        constant(InBoneWeights, "inBoneWeights[0]")

        namespace Block {
            constant(Name, "BoneSystem")
            constant(Bones, "bones")
            constant(BoneAttribsPerVertex, "boneAttribsPerVertex")
        }
    }
}
}
}

#undef constant

#endif //ALGINE_BONESYSTEM_CONSTANTS_H
