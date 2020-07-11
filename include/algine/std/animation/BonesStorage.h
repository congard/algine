#ifndef ALGINE_BONESSTORAGE_H
#define ALGINE_BONESSTORAGE_H

#include <algine/types.h>
#include <algine/std/animation/Bone.h>

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>

namespace algine {
class BonesStorage {
public:
    constexpr static uint BoneNotFound = -1;

public:
    void set(const std::string &name, const glm::mat4 &boneMatrix);
    void set(const Bone &bone);

    uint count() const;
    uint getIndex(const std::string &name) const;
    bool isExists(const std::string &name) const;

    Bone& operator[](uint index);

public:
    std::vector<Bone> bones;
};
}

#endif //ALGINE_BONESSTORAGE_H
