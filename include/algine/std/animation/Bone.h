#ifndef ALGINE_BONE_H
#define ALGINE_BONE_H

#include <string>
#include <glm/mat4x4.hpp>

namespace algine {
class Bone {
public:
    Bone(const std::string &name, const glm::mat4 &offsetMatrix);

public:
    std::string name;
    glm::mat4 offsetMatrix, finalTransformation;
};
}

#endif //ALGINE_BONE_H
