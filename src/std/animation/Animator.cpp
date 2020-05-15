#define GLM_FORCE_CTOR_INIT

#include <algine/std/animation/Animator.h>

#include <glm/gtx/quaternion.hpp>

namespace algine {
Animator::Animator() = default;

Animator::Animator(const AnimShape &shape, const usize animationIndex) {
    this->shape = shape;
    this->animationIndex = animationIndex;
}

void Animator::animate(const float timeInSeconds) {
    glm::mat4 identity;

    float ticksPerSecond = shape.animations->operator[](animationIndex).ticksPerSecond != 0 ? shape.animations->operator[](0).ticksPerSecond : 25.0f;

    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmodf(timeInTicks, static_cast<float>(shape.animations->operator[](animationIndex).duration));

    readNodeHeirarchy(animationTime, *shape.rootNode, identity);
}

usize Animator::findPosition(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->positionKeys.empty());

    for (usize i = 0; i < animNode->positionKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->positionKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

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

usize Animator::findRotation(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->rotationKeys.empty());

    for (usize i = 0; i < animNode->rotationKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->rotationKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

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

usize Animator::findScaling(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->scalingKeys.empty());

    for (usize i = 0; i < animNode->scalingKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->scalingKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

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

const AnimNode* Animator::findNodeAnim(const Animation *animation, const std::string &nodeName) {
    for (usize i = 0 ; i < animation->channels.size(); i++) {
        const AnimNode *channel = &animation->channels[i];

        if (channel->name == nodeName)
            return channel;
    }

    return nullptr;
}

void Animator::readNodeHeirarchy(const float animationTime, const Node &node, const glm::mat4 &parentTransform) {
    const std::string &nodeName = node.name;
    const Animation &animation = shape.animations->operator[](animationIndex);
    glm::mat4 nodeTransformation = node.defaultTransform * node.transformation; // WARNING: experimental feature "node.transformation"
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

    // WARNING: node.transformation: before or after nodeTransformation?
    // WARNING: experimental feature "node.transformation"
    glm::mat4 globalTransformation = parentTransform * nodeTransformation * node.transformation;

    for (usize i = 0; i < shape.bones->size(); i++) {
        if (shape.bones->operator[](i).name == nodeName) {
            shape.bones->operator[](i).finalTransformation = *shape.globalInverseTransform * globalTransformation * shape.bones->operator[](i).offsetMatrix;
            break;
        }
    }

    for (usize i = 0; i < node.childs.size(); i++)
        readNodeHeirarchy(animationTime, node.childs[i], globalTransformation);
}
}
