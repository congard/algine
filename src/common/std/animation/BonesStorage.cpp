#include <algine/std/animation/BonesStorage.h>

namespace algine {
void BonesStorage::set(const std::string &name, const glm::mat4 &boneMatrix) {
    uint index = getIndex(name);

    if (index == BoneNotFound) {
        m_bones.emplace_back(name, boneMatrix);
    } else {
        m_bones[index] = {name, boneMatrix};
    }
}

void BonesStorage::set(const Bone &bone) {
    set(bone.getName(), bone.getMatrix());
}

uint BonesStorage::size() const {
    return m_bones.size();
}

Index BonesStorage::getIndex(std::string_view name) const {
    for (Index i = 0; i < m_bones.size(); ++i) {
        if (m_bones[i].getName() == name) {
            return i;
        }
    }

    return BoneNotFound;
}

bool BonesStorage::isExists(std::string_view name) const {
    return getIndex(name) != BoneNotFound;
}

Bone& BonesStorage::operator[](Index index) {
    return m_bones[index];
}

std::vector<Bone>& BonesStorage::data() {
    return m_bones;
}
}
