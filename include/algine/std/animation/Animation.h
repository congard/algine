#ifndef ALGINE_ANIMATION_H
#define ALGINE_ANIMATION_H

#include <algine/std/animation/AnimNode.h>

#include <glm/mat4x4.hpp>

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
    std::vector<glm::mat4> bones;
};
}

#endif //ALGINE_ANIMATION_H
