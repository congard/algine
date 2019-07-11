#ifndef ALGINE_ANIMATION_H
#define ALGINE_ANIMATION_H

#include <assimp/anim.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algine/node.h>
#include <algine/bone.h>

namespace algine {
struct VecAnimKey {
    double time;
    glm::vec3 value;

    VecAnimKey(const aiVectorKey *key);

    float getTime();
};

struct QuatAnimKey {
    double time;
    glm::quat value;

    QuatAnimKey(const aiQuatKey *key);

    float getTime();
};

struct AnimNode {
    std::string name;
    std::vector<VecAnimKey> scalingKeys, positionKeys;
    std::vector<QuatAnimKey> rotationKeys;

    AnimNode(const aiNodeAnim *nodeAnim);
};

struct Animation {
    double ticksPerSecond, duration;
    std::string name;
    std::vector<AnimNode> channels;

    Animation(const aiAnimation *anim);
};

struct AnimShape {
    std::vector<Animation> *animations;
    std::vector<Bone> *bones;
    glm::mat4 *globalInverseTransform;
    Node *rootNode;

    AnimShape();

    AnimShape(std::vector<Animation> *animations, std::vector<Bone> *bones, glm::mat4 *globalInverseTransform, Node *rootNode);
};

struct Animator {
    AnimShape shape;
    usize animationIndex;

    Animator();

    Animator(const AnimShape &shape, const usize animationIndex = 0);

    void animate(const float timeInSeconds);

    static usize findPosition(const float animationTime, const AnimNode *animNode);

    static void calcInterpolatedPosition(glm::vec3 &out, const float animationTime, const AnimNode *animNode);

    static usize findRotation(const float animationTime, const AnimNode *animNode);

    static void calcInterpolatedRotation(glm::quat &out, const float animationTime, const AnimNode *animNode);

    static usize findScaling(const float animationTime, const AnimNode *animNode);

    static void calcInterpolatedScaling(glm::vec3 &out, const float animationTime, const AnimNode *animNode);

    static const AnimNode* findNodeAnim(const Animation *animation, const std::string &nodeName);

    void readNodeHeirarchy(const float animationTime, const Node &node, const glm::mat4 &parentTransform);
};

} /* namespace algine */

#endif /* ALGINE_ANIMATION_H */