#include <algine/std/animation/Bone.h>

#include <utility>

namespace algine {
Bone::Bone(std::string boneName, const glm::mat4 &matrix)
    : name(std::move(boneName)),
      boneMatrix(matrix),
      transformation(1.0)
{
    // see initializer list above
}
}
