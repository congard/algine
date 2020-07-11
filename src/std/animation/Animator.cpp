#define GLM_FORCE_CTOR_INIT
#include <algine/std/animation/Animator.h>

#include <stdexcept>
#include <glm/gtx/quaternion.hpp>

using namespace std;

namespace algine {
Animator::Animator() = default;

Animator::Animator(Shape *const shape, const std::string &animationName) {
    m_shape = shape;
    setAnimation(animationName);
}

Animator::Animator(Shape *const shape, const usize animationIndex) {
    m_shape = shape;
    m_animationIndex = animationIndex;
}

void Animator::animate(const float timeInSeconds) {
    glm::mat4 identity;

    const auto animTicksPerSecond = static_cast<float>(m_shape->animations[m_animationIndex].ticksPerSecond);
    float ticksPerSecond = animTicksPerSecond != 0 ? animTicksPerSecond : 25.0f;

    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmodf(timeInTicks, static_cast<float>(m_shape->animations[m_animationIndex].duration));

    readNodeHierarchy(animationTime, m_shape->rootNode, identity);
}

void Animator::setShape(Shape *const shape) {
    m_shape = shape;
}

void Animator::setAnimationIndex(const usize animationIndex) {
    m_animationIndex = animationIndex;
}

void Animator::setAnimation(const std::string &name) {
    m_animationIndex = m_shape->getAnimationIndexByName(name);

    if (m_animationIndex == Shape::AnimationNotFound) {
        string available;

        for (uint i = 0; i < m_shape->animations.size(); i++)
            available += "  " + to_string(i) + ". '" + m_shape->animations[i].name + "'\n";

        throw runtime_error("Animation '" + name + "' not found\nAvailable animations:\n" + available);
    }
}

Shape* Animator::getShape() const {
    return m_shape;
}

usize Animator::getAnimationIndex() const {
    return m_animationIndex;
}

inline usize findPosition(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->positionKeys.empty());

    for (usize i = 0; i < animNode->positionKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->positionKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedPosition(glm::vec3 &out, const float animationTime, const AnimNode *animNode) {
    if (animNode->positionKeys.size() == 1) {
        out = animNode->positionKeys[0].value;
        return;
    }

    usize positionIndex = findPosition(animationTime, animNode);
    usize nextPositionIndex = positionIndex + 1;
    assert(nextPositionIndex < animNode->positionKeys.size());
    auto deltaTime = (float)(animNode->positionKeys[nextPositionIndex].time - animNode->positionKeys[positionIndex].time);
    auto factor = (animationTime - (float)animNode->positionKeys[positionIndex].time) / deltaTime;
#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif
    const glm::vec3 &start = animNode->positionKeys[positionIndex].value;
    const glm::vec3 &end = animNode->positionKeys[nextPositionIndex].value;
    glm::vec3 delta = end - start;
    out = start + factor * delta;
}

inline usize findRotation(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->rotationKeys.empty());

    for (usize i = 0; i < animNode->rotationKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->rotationKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedRotation(glm::quat &out, const float animationTime, const AnimNode *animNode) {
    // we need at least two values to interpolate...
    if (animNode->rotationKeys.size() == 1) {
        out = animNode->rotationKeys[0].value;
        return;
    }

    usize rotationIndex = findRotation(animationTime, animNode);
    usize nextRotationIndex = rotationIndex + 1;
    assert(nextRotationIndex < animNode->rotationKeys.size());
    auto deltaTime = (float)(animNode->rotationKeys[nextRotationIndex].time - animNode->rotationKeys[rotationIndex].time);
    auto factor = (animationTime - (float)animNode->rotationKeys[rotationIndex].time) / deltaTime;
#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif
    const glm::quat &startRotationQ = animNode->rotationKeys[rotationIndex].value;
    const glm::quat &endRotationQ   = animNode->rotationKeys[nextRotationIndex].value;
    out = glm::slerp(startRotationQ, endRotationQ, factor); // aiQuaternion::Interpolate
    out = glm::normalize(out);
}

inline usize findScaling(const float animationTime, const AnimNode *animNode) {
    assert(!animNode->scalingKeys.empty());

    for (usize i = 0; i < animNode->scalingKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->scalingKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedScaling(glm::vec3 &out, const float animationTime, const AnimNode *animNode) {
    if (animNode->scalingKeys.size() == 1) {
        out = animNode->scalingKeys[0].value;
        return;
    }

    usize scalingIndex = findScaling(animationTime, animNode);
    usize nextScalingIndex = scalingIndex + 1;
    assert(nextScalingIndex < animNode->scalingKeys.size());
    auto deltaTime = (float)(animNode->scalingKeys[nextScalingIndex].time - animNode->scalingKeys[scalingIndex].time);
    auto factor = (animationTime - (float)animNode->scalingKeys[scalingIndex].time) / deltaTime;
#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif
    const glm::vec3 &start = animNode->scalingKeys[scalingIndex].value;
    const glm::vec3 &end = animNode->scalingKeys[nextScalingIndex].value;
    glm::vec3 delta = end - start;
    out = start + factor * delta;
}

inline const AnimNode* findNodeAnim(const Animation *animation, const std::string &nodeName) {
    for (usize i = 0 ; i < animation->channels.size(); i++) {
        const AnimNode *channel = &animation->channels[i];

        if (channel->name == nodeName)
            return channel;
    }

    return nullptr;
}

void Animator::readNodeHierarchy(const float animationTime, const Node &node, const glm::mat4 &parentTransform) {
    const std::string &nodeName = node.name;
    const Animation &animation = m_shape->animations[m_animationIndex];
    glm::mat4 nodeTransformation = node.defaultTransform;
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

    for (auto & bone : m_shape->bones) {
        if (bone.name == nodeName) {
            bone.finalTransformation = m_shape->globalInverseTransform * globalTransformation * bone.offsetMatrix;
            break;
        }
    }

    for (const auto & child : node.childs)
        readNodeHierarchy(animationTime, child, globalTransformation);
}
}
