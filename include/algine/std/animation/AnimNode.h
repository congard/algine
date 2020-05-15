#ifndef ALGINE_ANIMNODE_H
#define ALGINE_ANIMNODE_H

#include <algine/std/animation/VecAnimKey.h>
#include <algine/std/animation/QuatAnimKey.h>

#include <string>
#include <vector>

class aiNodeAnim;

namespace algine {
class AnimNode {
public:
    explicit AnimNode(const aiNodeAnim *nodeAnim);

public:
    std::string name;
    std::vector<VecAnimKey> scalingKeys, positionKeys;
    std::vector<QuatAnimKey> rotationKeys;
};
}

#endif //ALGINE_ANIMNODE_H
