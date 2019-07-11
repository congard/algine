#define GLM_FORCE_CTOR_INIT

#include <algine/animation.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <vector>
#include <glm/gtx/quaternion.hpp>

#include <algine/types.h>

#include <iostream>

#ifdef ALGINE_ANIMATION_FACTOR_ASSERTION
#define mkAssert ALGINE_ANIMATION_ASSERTION
#endif

namespace algine {
// struct VecAnimKey
VecAnimKey::VecAnimKey(const aiVectorKey *key) {
    time = key->mTime;
    value = glm::vec3(key->mValue.x, key->mValue.y, key->mValue.z);
}

float VecAnimKey::getTime() {
    return (float)time;
}

// struct QuatAnimKey
QuatAnimKey::QuatAnimKey(const aiQuatKey *key) {
    time = key->mTime;
    value = glm::quat(key->mValue.w, key->mValue.x, key->mValue.y, key->mValue.z);
}

float QuatAnimKey::getTime() {
    return (float)time;
}

// struct AnimNode
AnimNode::AnimNode(const aiNodeAnim *nodeAnim) {
    name = nodeAnim->mNodeName.data;
    // allocating space for scaling, position, rotation keys
    scalingKeys.reserve(nodeAnim->mNumScalingKeys);
    positionKeys.reserve(nodeAnim->mNumPositionKeys);
    rotationKeys.reserve(nodeAnim->mNumRotationKeys);
    // filling arrays
    for (size_t i = 0; i < nodeAnim->mNumScalingKeys; i++) scalingKeys.push_back(&nodeAnim->mScalingKeys[i]);
    for (size_t i = 0; i < nodeAnim->mNumPositionKeys; i++) positionKeys.push_back(&nodeAnim->mPositionKeys[i]);
    for (size_t i = 0; i < nodeAnim->mNumRotationKeys; i++) rotationKeys.push_back(&nodeAnim->mRotationKeys[i]);
}

// struct Animation
Animation::Animation(const aiAnimation *anim) {
    ticksPerSecond = anim->mTicksPerSecond;
    duration = anim->mDuration;
    name = anim->mName.data;
    channels.reserve(anim->mNumChannels); // allocate space
    for (size_t i = 0; i < anim->mNumChannels; i++) channels.push_back(AnimNode(anim->mChannels[i]));
}

// struct AnimShape
AnimShape::AnimShape() { /* empty */ }

AnimShape::AnimShape(std::vector<Animation> *animations, std::vector<Bone> *bones, glm::mat4 *globalInverseTransform, Node *rootNode) {
    this->animations = animations;
    this->bones = bones;
    this->globalInverseTransform = globalInverseTransform;
    this->rootNode = rootNode;
}

// struct Animator
Animator::Animator() { /* empty */ }

Animator::Animator(const AnimShape &shape, const usize animationIndex) {
    this->shape = shape;
    this->animationIndex = animationIndex;
}

void Animator::animate(const float timeInSeconds) {
    glm::mat4 identity;

    float ticksPerSecond = shape.animations->operator[](animationIndex).ticksPerSecond != 0 ? shape.animations->operator[](0).ticksPerSecond : 25.0f;

    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, shape.animations->operator[](animationIndex).duration);

    readNodeHeirarchy(animationTime, *shape.rootNode, identity);
}

// static
usize Animator::findPosition(const float animationTime, const AnimNode *animNode) {
    assert(animNode->positionKeys.size() > 0);
        
    for (usize i = 0; i < animNode->positionKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->positionKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);

    return 0;
}

// static
void Animator::calcInterpolatedPosition(glm::vec3 &out, const float animationTime, const AnimNode *animNode) {
    if (animNode->positionKeys.size() == 1) {
        out = animNode->positionKeys[0].value;
        return;
    }

    usize positionIndex = findPosition(animationTime, animNode);
    usize nextPositionIndex = positionIndex + 1;
    assert(nextPositionIndex < animNode->positionKeys.size());
    float deltaTime = (float)(animNode->positionKeys[nextPositionIndex].time - animNode->positionKeys[positionIndex].time);
    float factor = (animationTime - (float)animNode->positionKeys[positionIndex].time) / deltaTime;
    #ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
    #endif
    const glm::vec3 &start = animNode->positionKeys[positionIndex].value;
    const glm::vec3 &end = animNode->positionKeys[nextPositionIndex].value;
    glm::vec3 delta = end - start;
    out = start + factor * delta;
}

// static
usize Animator::findRotation(const float animationTime, const AnimNode *animNode) {
    assert(animNode->rotationKeys.size() > 0);
        
    for (usize i = 0; i < animNode->rotationKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->rotationKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);

    return 0;
}

// static
void Animator::calcInterpolatedRotation(glm::quat &out, const float animationTime, const AnimNode *animNode) {
    // we need at least two values to interpolate...
    if (animNode->rotationKeys.size() == 1) {
        out = animNode->rotationKeys[0].value;
        return;
    }

    usize rotationIndex = findRotation(animationTime, animNode);
    usize nextRotationIndex = rotationIndex + 1;
    assert(nextRotationIndex < animNode->rotationKeys.size());
    float deltaTime = (float)(animNode->rotationKeys[nextRotationIndex].time - animNode->rotationKeys[rotationIndex].time);
    float factor = (animationTime - (float)animNode->rotationKeys[rotationIndex].time) / deltaTime;
    #ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
    #endif
    const glm::quat &startRotationQ = animNode->rotationKeys[rotationIndex].value;
    const glm::quat &endRotationQ   = animNode->rotationKeys[nextRotationIndex].value;
    out = glm::slerp(startRotationQ, endRotationQ, factor); // aiQuaternion::Interpolate
    out = glm::normalize(out);
}

// static
usize Animator::findScaling(const float animationTime, const AnimNode *animNode) {
    assert(animNode->scalingKeys.size() > 0);
       
    for (usize i = 0; i < animNode->scalingKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->scalingKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);

    return 0;
}

// static
void Animator::calcInterpolatedScaling(glm::vec3 &out, const float animationTime, const AnimNode *animNode) {
    if (animNode->scalingKeys.size() == 1) {
        out = animNode->scalingKeys[0].value;
        return;
    }

    usize scalingIndex = findScaling(animationTime, animNode);
    usize nextScalingIndex = scalingIndex + 1;
    assert(nextScalingIndex < animNode->scalingKeys.size());
    float deltaTime = (float)(animNode->scalingKeys[nextScalingIndex].time - animNode->scalingKeys[scalingIndex].time);
    float factor = (animationTime - (float)animNode->scalingKeys[scalingIndex].time) / deltaTime;
    #ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
    #endif
    const glm::vec3 &start = animNode->scalingKeys[scalingIndex].value;
    const glm::vec3 &end = animNode->scalingKeys[nextScalingIndex].value;
    glm::vec3 delta = end - start;
    out = start + factor * delta;
}

// static
const AnimNode* Animator::findNodeAnim(const Animation *animation, const std::string &nodeName) {
    for (usize i = 0 ; i < animation->channels.size(); i++) {
        const AnimNode *channel = &animation->channels[i];

        if (channel->name == nodeName) {
            return channel;
        }
    }

    return nullptr;
}

void Animator::readNodeHeirarchy(const float animationTime, const Node &node, const glm::mat4 &parentTransform) {
    const std::string &nodeName = node.name;
    const Animation &animation = shape.animations->operator[](animationIndex);
    glm::mat4 nodeTransformation = node.transformation;
    const AnimNode *animNode = findNodeAnim(&animation, nodeName);

    if (animNode) {
        // Интерполируем масштабирование и генерируем матрицу преобразования масштаба
        glm::vec3 scaling;
        calcInterpolatedScaling(scaling, animationTime, animNode);
        glm::mat4 scalingM;
        scalingM = glm::scale(scalingM, scaling);

        // Интерполируем вращение и генерируем матрицу вращения
        glm::quat rotationQ;
        calcInterpolatedRotation(rotationQ, animationTime, animNode);
        glm::mat4 rotationM = glm::toMat4(rotationQ);

        //  Интерполируем смещение и генерируем матрицу смещения
        glm::vec3 translation;
        calcInterpolatedPosition(translation, animationTime, animNode);
        glm::mat4 translationM;
        translationM = glm::translate(translationM, translation);
            
        // Объединяем преобразования
        nodeTransformation = translationM * rotationM * scalingM;
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransformation;

    for (usize i = 0; i < shape.bones->size(); i++) {
        if (shape.bones->operator[](i).name == nodeName) {
            shape.bones->operator[](i).finalTransformation = *shape.globalInverseTransform * globalTransformation * shape.bones->operator[](i).offsetMatrix;
            break;
        }
    }

    for (usize i = 0; i < node.childs.size(); i++) {
        readNodeHeirarchy(animationTime, node.childs[i], globalTransformation);
    }
}

} /* namespace algine */

#ifdef ALGINE_ANIMATION_ASSERTION
#undef mkAssert
#endif