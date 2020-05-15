#include <algine/std/animation/BoneInfo.h>

using namespace tulz;

namespace algine {
BoneInfo::BoneInfo(const uint size) {
    ids = Array<uint>(size);
    weights = Array<float>(size);

    for (uint i = 0; i < size; i++) {
        ids[i] = 0;
        weights[i] = 0;
    }
}

void BoneInfo::add(const uint id, const float weight) {
    for (uint i = 0 ; i < ids.m_size; i++) {
        if (weights[i] == 0.0) {
            ids[i] = id;
            weights[i] = weight;
            return;
        }
    }
}

uint BoneInfo::size() const {
    return ids.m_size;
}
}
