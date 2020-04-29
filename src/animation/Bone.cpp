#include <algine/animation/Bone.h>

namespace algine {
Bone::Bone(const std::string &name, const glm::mat4 &offsetMatrix) {
    this->name = name;
    this->offsetMatrix = offsetMatrix;
}
}
