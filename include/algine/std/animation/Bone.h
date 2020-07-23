#ifndef ALGINE_BONE_H
#define ALGINE_BONE_H

#include <string>
#include <glm/mat4x4.hpp>

namespace algine {
class Bone {
public:
    Bone(const std::string &name, const glm::mat4 &matrix);

public:
    std::string name;
    glm::mat4 boneMatrix;
};
}

#endif //ALGINE_BONE_H