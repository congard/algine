#include <algine/std/animation/Bone.h>

#include <utility>

namespace algine {
Bone::Bone(std::string boneName, const BoneMatrix &matrix)
    : m_name(std::move(boneName)),
      m_boneMatrix(matrix) {}
}
