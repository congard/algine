#ifndef ALGINE_BONESYSTEM_H
#define ALGINE_BONESYSTEM_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace BoneSystem {
        namespace Settings {
            constant(MaxBoneAttribsPerVertex, "MAX_BONE_ATTRIBS_PER_VERTEX")
            constant(MaxBones, "MAX_BONES")
        }

        namespace Vars {
            constant(InBoneIds, "inBoneIds[0]") // integer
            constant(InBoneWeights, "inBoneWeights[0]")
            constant(Bones, "bones[0]") // zero element, so ShaderProgram.getLocation(Bones) + i, not bones[i]
            constant(BoneAttribsPerVertex, "boneAttribsPerVertex") // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)
        }
    }
}

#undef constant

#endif //ALGINE_BONESYSTEM_H
