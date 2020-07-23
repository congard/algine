#include <algine/std/animation/BoneInfo.h>

using namespace tulz;
using namespace std;

#define m_id first
#define m_weight second

namespace algine {
BoneInfo::BoneInfo(const uint size) {
    info = Array<pair<uint, float>>(size);

    for (uint i = 0; i < size; i++) {
        info[i].m_id = 0;
        info[i].m_weight = 0;
    }
}

inline bool sortingComparator(const pair<uint, float> &p1, const pair<uint, float> &p2) {
    return p1.m_weight > p2.m_weight;
}

void BoneInfo::sort() {
    std::sort(&info[0], &info[0] + info.size(), sortingComparator);
}

void BoneInfo::add(const uint id, const float weight) {
    for (uint i = 0 ; i < info.m_size; i++) {
        if (info[i].m_weight == 0.0) {
            info[i].m_id = id;
            info[i].m_weight = weight;
            return;
        }
    }
}

void BoneInfo::setId(const uint index, const uint id) {
    info[index].m_id = id;
}

void BoneInfo::setWeight(const uint index, const float weight) {
    info[index].m_weight = weight;
}

uint BoneInfo::size() const {
    return info.m_size;
}

uint BoneInfo::getId(const uint index) const {
    return info.array()[index].m_id;
}

float BoneInfo::getWeight(const uint index) const {
    return info.array()[index].m_weight;
}
}
