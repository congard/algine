#include <algine/std/animation/BonesStorage.h>

namespace algine {
void BonesStorage::set(const std::string &name, const glm::mat4 &boneMatrix) {
    uint index = getIndex(name);

    if (index == BoneNotFound) {
        m_bones.emplace_back(name, boneMatrix);
    } else {
        m_bones[index].boneMatrix = boneMatrix;
    }
}

void BonesStorage::set(const Bone &bone) {
    set(bone.name, bone.boneMatrix);
}

uint BonesStorage::size() const {
    return m_bones.size();
}

Index BonesStorage::getIndex(const std::string &name) const {
    for (Index i = 0; i < m_bones.size(); ++i) {
        if (m_bones[i].name == name) {
            return i;
        }
    }

    return BoneNotFound;
}

bool BonesStorage::isExists(const std::string &name) const {
    return getIndex(name) != BoneNotFound;
}

Bone& BonesStorage::operator[](Index index) {
    return m_bones[index];
}

std::vector<Bone>& BonesStorage::data() {
    return m_bones;
}
}
