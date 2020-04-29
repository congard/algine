#ifndef ALGINE_ANIMATOR_H
#define ALGINE_ANIMATOR_H

#include <algine/animation/AnimShape.h>
#include <algine/types.h>

namespace algine {
class Animator {
public:
    Animator();
    explicit Animator(const AnimShape &shape, usize animationIndex = 0);

    void animate(float timeInSeconds);
    void readNodeHeirarchy(float animationTime, const Node &node, const glm::mat4 &parentTransform);

    static usize findPosition(float animationTime, const AnimNode *animNode);
    static usize findRotation(float animationTime, const AnimNode *animNode);
    static usize findScaling(float animationTime, const AnimNode *animNode);
    static void calcInterpolatedPosition(glm::vec3 &out, float animationTime, const AnimNode *animNode);
    static void calcInterpolatedRotation(glm::quat &out, float animationTime, const AnimNode *animNode);
    static void calcInterpolatedScaling(glm::vec3 &out, float animationTime, const AnimNode *animNode);
    static const AnimNode* findNodeAnim(const Animation *animation, const std::string &nodeName);

public:
    AnimShape shape;
    usize animationIndex;
};
}

#endif //ALGINE_ANIMATOR_H
