#include <algine/std/animation/Bone.h>

#include <utility>

namespace algine {
Bone::Bone(std::string boneName, const BoneMatrix &matrix)
    : name(std::move(boneName)),
      boneMatrix(matrix) {}
}
