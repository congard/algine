#include <algine/std/animation/BonesStorage.h>

namespace algine {
void BonesStorage::set(const std::string &name, const glm::mat4 &boneMatrix) {
    uint index = getIndex(name);

    if (index == BoneNotFound)
        bones.emplace_back(name, boneMatrix);
    else
        bones[index].boneMatrix = boneMatrix;
}

void BonesStorage::set(const Bone &bone) {
    set(bone.name, bone.boneMatrix);
}

uint BonesStorage::count() const {
    return bones.size();
}

uint BonesStorage::getIndex(const std::string &name) const {
    for (uint i = 0; i < bones.size(); ++i)
        if (bones[i].name == name)
            return i;

    return BoneNotFound;
}

bool BonesStorage::isExists(const std::string &name) const {
    return getIndex(name) != BoneNotFound;
}

Bone& BonesStorage::operator[](const uint index) {
    return bones[index];
}
}
