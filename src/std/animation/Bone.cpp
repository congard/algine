#include <algine/std/animation/Bone.h>

namespace algine {
Bone::Bone(const std::string &boneName, const glm::mat4 &matrix) {
    name = boneName;
    boneMatrix = matrix;
}
}
