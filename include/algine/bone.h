#ifndef ALGINE_BONE_H
#define ALGINE_BONE_H

#include <algine/types.h>
#include <string>
#include <glm/mat4x4.hpp>

namespace algine {
struct BoneInfo {
    uint *ids = nullptr, size;
    float *weights = nullptr;

    BoneInfo(const uint &size);

    void add(const uint &id, const float &weight);

    // copy constructor
    BoneInfo(const BoneInfo &src);

    // move constructor
    BoneInfo(BoneInfo &&src);

    ~BoneInfo();

private:
    void fill();
};

struct Bone {
    std::string name;
    glm::mat4 offsetMatrix, finalTransformation;

    Bone(const std::string &name, const glm::mat4 &offsetMatrix);
};

} /* namespace algine */

#endif /* ALGINE_BONE_H */