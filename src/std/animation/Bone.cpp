#include <algine/std/animation/Bone.h>

namespace algine {
Bone::Bone(const std::string &name, const glm::mat4 &matrix) {
    this->name = name;
    this->boneMatrix = matrix;
}
}
