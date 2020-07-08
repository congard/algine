#ifndef ALGINE_BONEINFO_H
#define ALGINE_BONEINFO_H

#include <algine/types.h>

#include <tulz/Array.h>

namespace algine {
class BoneInfo {
public:
    explicit BoneInfo(uint size);

    void add(uint id, float weight);
    uint size() const;

public:
    tulz::Array<uint> ids;
    tulz::Array<float> weights;
};
}

#endif //ALGINE_BONEINFO_H
