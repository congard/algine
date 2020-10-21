#define GLM_FORCE_CTOR_INIT
#include <algine/std/animation/Animator.h>

#include <glm/gtx/quaternion.hpp>

#include <stdexcept>

using namespace std;
using namespace glm;

namespace algine {
Animator::Animator() = default;

Animator::Animator(Shape *shape, const string &animationName) {
    m_shape = shape;
    setAnimation(animationName);
}

Animator::Animator(Shape *shape, usize animationIndex) {
    m_shape = shape;
    m_animationIndex = animationIndex;
}

void Animator::animate(float timeInSeconds) {
    mat4 identity;

    const auto &animation = m_shape->animations[m_animationIndex];

    auto animTicksPerSecond = static_cast<float>(animation.ticksPerSecond);
    float ticksPerSecond = animTicksPerSecond != 0 ? animTicksPerSecond : 25.0f;

    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmodf(timeInTicks, static_cast<float>(animation.duration));

    readNodeHierarchy(animationTime, m_shape->rootNode, identity);
}

void Animator::setShape(Shape *shape) {
    m_shape = shape;
}

void Animator::setAnimationIndex(usize animationIndex) {
    m_animationIndex = animationIndex;
}

void Animator::setAnimation(const string &name) {
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

inline usize findPosition(float animationTime, const AnimNode *animNode) {
    assert(!animNode->positionKeys.empty());

    for (usize i = 0; i < animNode->positionKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->positionKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedPosition(vec3 &out, float animationTime, const AnimNode *animNode) {
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

    const vec3 &start = animNode->positionKeys[positionIndex].value;
    const vec3 &end = animNode->positionKeys[nextPositionIndex].value;
    vec3 delta = end - start;

    out = start + factor * delta;
}

inline usize findRotation(float animationTime, const AnimNode *animNode) {
    assert(!animNode->rotationKeys.empty());

    for (usize i = 0; i < animNode->rotationKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->rotationKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedRotation(quat &out, float animationTime, const AnimNode *animNode) {
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

    const quat &startRotationQ = animNode->rotationKeys[rotationIndex].value;
    const quat &endRotationQ   = animNode->rotationKeys[nextRotationIndex].value;

    out = slerp(startRotationQ, endRotationQ, factor); // aiQuaternion::Interpolate
    out = normalize(out);
}

inline usize findScaling(float animationTime, const AnimNode *animNode) {
    assert(!animNode->scalingKeys.empty());

    for (usize i = 0; i < animNode->scalingKeys.size() - 1; i++) {
        if (animationTime < (float)animNode->scalingKeys[i + 1].time) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedScaling(vec3 &out, float animationTime, const AnimNode *animNode) {
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

    const vec3 &start = animNode->scalingKeys[scalingIndex].value;
    const vec3 &end = animNode->scalingKeys[nextScalingIndex].value;
    vec3 delta = end - start;

    out = start + factor * delta;
}

inline const AnimNode* findNodeAnim(const Animation *animation, const string &nodeName) {
    for (usize i = 0 ; i < animation->channels.size(); i++) {
        const AnimNode *channel = &animation->channels[i];

        if (channel->name == nodeName) {
            return channel;
        }
    }

    return nullptr;
}

void Animator::readNodeHierarchy(float animationTime, const Node &node, const mat4 &parentTransform) {
    const string &nodeName = node.name;
    const Animation &animation = m_shape->animations[m_animationIndex];
    const AnimNode *animNode = findNodeAnim(&animation, nodeName);
    mat4 nodeTransformation = node.defaultTransform;

    if (animNode) {
        // Интерполируем масштабирование и генерируем матрицу преобразования масштаба
        vec3 scaling;
        calcInterpolatedScaling(scaling, animationTime, animNode);

        mat4 scalingM = scale(mat4(1.0), scaling);

        // Интерполируем вращение и генерируем матрицу вращения
        quat rotationQ;
        calcInterpolatedRotation(rotationQ, animationTime, animNode);

        mat4 rotationM = toMat4(rotationQ);

        //  Интерполируем смещение и генерируем матрицу смещения
        vec3 translation;
        calcInterpolatedPosition(translation, animationTime, animNode);

        mat4 translationM = translate(mat4(1.0), translation);

        // Объединяем преобразования
        nodeTransformation = translationM * rotationM * scalingM;
    }

    mat4 globalTransformation = parentTransform * nodeTransformation;

    if (uint index = m_shape->bonesStorage.getIndex(nodeName); index != BonesStorage::BoneNotFound) {
        auto &bone = m_shape->bonesStorage.bones[index];
        auto &dstBone = m_shape->animations[m_animationIndex].bones[index];

        globalTransformation *= bone.transformation;
        dstBone = m_shape->globalInverseTransform * globalTransformation * bone.boneMatrix;
    }

    for (const auto & child : node.childs) {
        readNodeHierarchy(animationTime, child, globalTransformation);
    }
}
}
