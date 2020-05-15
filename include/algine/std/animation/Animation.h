#ifndef ALGINE_ANIMATION_H
#define ALGINE_ANIMATION_H

#include <algine/std/animation/AnimNode.h>
#include <string>
#include <vector>

class aiAnimation;

namespace algine {
class Animation {
public:
    explicit Animation(const aiAnimation *anim);

public:
    double ticksPerSecond, duration;
    std::string name;
    std::vector<AnimNode> channels;
};
}

#endif //ALGINE_ANIMATION_H
