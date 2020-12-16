#ifndef ALGINE_BONE_H
#define ALGINE_BONE_H

#include <algine/std/animation/BoneMatrix.h>

#include <string>

namespace algine {
class Bone {
public:
    Bone(std::string name, const BoneMatrix &matrix);

public:
    std::string name;
    BoneMatrix boneMatrix;
};
}

#endif //ALGINE_BONE_H
