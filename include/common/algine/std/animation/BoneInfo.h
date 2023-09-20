#ifndef ALGINE_BONEINFO_H
#define ALGINE_BONEINFO_H

#include <algine/platform.h>
#include <algine/types.h>

#include <tulz/Array.h>
#include <utility>

namespace algine {
class AL_EXPORT BoneInfo {
public:
    explicit BoneInfo(uint size);

    void sort();
    void add(uint id, float weight);
    void setId(uint index, uint id);
    void setWeight(uint index, float weight);

    uint size() const;
    uint getId(uint index) const;
    float getWeight(uint index) const;

public:
    tulz::Array<std::pair<uint, float>> info; // id, weight
};
}

#endif //ALGINE_BONEINFO_H
