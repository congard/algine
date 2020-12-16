#ifndef ALGINE_BONESSTORAGE_H
#define ALGINE_BONESSTORAGE_H

#include <algine/std/animation/Bone.h>
#include <algine/types.h>

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>

namespace algine {
class BonesStorage {
public:
    constexpr static Index BoneNotFound = -1;

public:
    void set(const std::string &name, const glm::mat4 &boneMatrix);
    void set(const Bone &bone);

    uint size() const;
    Index getIndex(const std::string &name) const;
    bool isExists(const std::string &name) const;

    Bone& operator[](Index index);

    std::vector<Bone>& data();

private:
    std::vector<Bone> m_bones;
};
}

#endif //ALGINE_BONESSTORAGE_H
