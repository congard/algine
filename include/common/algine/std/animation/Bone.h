#ifndef ALGINE_BONE_H
#define ALGINE_BONE_H

#include <algine/std/animation/BoneMatrix.h>
#include <algine/platform.h>

#include <string>

namespace algine {
class AL_EXPORT Bone {
public:
    Bone(std::string name, const BoneMatrix &matrix);

    inline const std::string& getName() const;
    inline const BoneMatrix& getMatrix() const;

private:
    std::string m_name;
    BoneMatrix m_boneMatrix;
};

const std::string& Bone::getName() const {
    return m_name;
}

const BoneMatrix& Bone::getMatrix() const {
    return m_boneMatrix;
}
}

#endif //ALGINE_BONE_H
