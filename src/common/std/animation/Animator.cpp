#include <algine/std/animation/Animator.h>

#include <algine/std/model/Model.h>
#include <algine/std/model/Shape.h>

#include <glm/gtx/quaternion.hpp>

#include <stdexcept>

using namespace std;
using namespace glm;

namespace algine {
Animator::Animator() = default;

Animator::Animator(Model *model, const string &animationName)
    : m_model(model)
{
    setAnimation(animationName);
}

Animator::Animator(Model *model, Index animationIndex)
    : m_model(model),
      m_animationIndex(animationIndex) {}

void Animator::animate(float timeInSeconds) {
    mat4 identity {1.0f};

    const auto &shape = m_model->getShape();
    const auto &animation = shape->getAnimation(m_animationIndex);

    float animTicksPerSecond = animation.getTicksPerSecond();
    float ticksPerSecond = animTicksPerSecond != 0 ? animTicksPerSecond : 25.0f;

    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmodf(timeInTicks, animation.getDuration());

    readNodeHierarchy(animationTime, shape->getRootNode(), identity);
}

void Animator::setModel(Model *model) {
    m_model = model;
}

void Animator::setAnimationIndex(Index animationIndex) {
    m_animationIndex = animationIndex;
}

void Animator::setAnimation(const string &name) {
    const auto &shape = m_model->getShape();

    m_animationIndex = shape->getAnimationIndexByName(name);

    if (m_animationIndex == Shape::AnimationNotFound) {
        string available;

        for (uint i = 0; i < shape->getAnimationsAmount(); i++)
            available += "  " + to_string(i) + ". '" + shape->getAnimation(i).getName() + "'\n";

        throw runtime_error("Animation '" + name + "' not found\nAvailable animations:\n" + available);
    }
}

Model* Animator::getModel() const {
    return m_model;
}

Index Animator::getAnimationIndex() const {
    return m_animationIndex;
}

inline usize findPosition(float animationTime, const AnimNode *animNode) {
    assert(!animNode->getPositionKeys().empty());

    for (usize i = 0; i < animNode->positionKeysCount() - 1; i++) {
        if (animationTime < animNode->getScalingKey(i + 1).getTime()) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedPosition(vec3 &out, float animationTime, const AnimNode *animNode) {
    if (animNode->positionKeysCount() == 1) {
        out = animNode->getPositionKey(0).getValue();
        return;
    }

    usize positionIndex = findPosition(animationTime, animNode);
    usize nextPositionIndex = positionIndex + 1;

    assert(nextPositionIndex < animNode->getPositionKeys().size());

    const auto &positionKey = animNode->getPositionKey(positionIndex);
    const auto &nextPositionKey = animNode->getPositionKey(nextPositionIndex);

    auto keyTime = positionKey.getTime();
    auto nextKeyTime = nextPositionKey.getTime();

    auto deltaTime = nextKeyTime - keyTime;
    auto factor = (animationTime - keyTime) / deltaTime;

#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif

    const vec3 &start = positionKey.getValue();
    const vec3 &end = nextPositionKey.getValue();
    vec3 delta = end - start;

    out = start + factor * delta;
}

inline usize findRotation(float animationTime, const AnimNode *animNode) {
    assert(!animNode->getRotationKeys().empty());

    for (usize i = 0; i < animNode->rotationKeysCount() - 1; i++) {
        if (animationTime < animNode->getRotationKey(i + 1).getTime()) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedRotation(quat &out, float animationTime, const AnimNode *animNode) {
    // we need at least two values to interpolate
    if (animNode->rotationKeysCount() == 1) {
        out = animNode->getRotationKey(0).getValue();
        return;
    }

    usize rotationIndex = findRotation(animationTime, animNode);
    usize nextRotationIndex = rotationIndex + 1;

    assert(nextRotationIndex < animNode->rotationKeysCount());

    const auto &rotationKey = animNode->getRotationKey(rotationIndex);
    const auto &nextRotationKey = animNode->getRotationKey(nextRotationIndex);

    auto keyTime = rotationKey.getTime();
    auto nextKeyTime = nextRotationKey.getTime();

    auto deltaTime = nextKeyTime - keyTime;
    auto factor = (animationTime - keyTime) / deltaTime;

#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif

    const quat &startRotationQ = rotationKey.getValue();
    const quat &endRotationQ = nextRotationKey.getValue();

    out = slerp(startRotationQ, endRotationQ, factor); // aiQuaternion::Interpolate
    out = normalize(out);
}

inline usize findScaling(float animationTime, const AnimNode *animNode) {
    assert(!animNode->getScalingKeys().empty());

    for (usize i = 0; i < animNode->scalingKeysCount() - 1; i++) {
        if (animationTime < animNode->getScalingKey(i + 1).getTime()) {
            return i;
        }
    }

    assert(0);
}

inline void calcInterpolatedScaling(vec3 &out, float animationTime, const AnimNode *animNode) {
    if (animNode->scalingKeysCount() == 1) {
        out = animNode->getScalingKey(0).getValue();
        return;
    }

    usize scalingIndex = findScaling(animationTime, animNode);
    usize nextScalingIndex = scalingIndex + 1;

    assert(nextScalingIndex < animNode->scalingKeysCount());

    const auto &scalingKey = animNode->getScalingKey(scalingIndex);
    const auto &nextScalingKey = animNode->getScalingKey(nextScalingIndex);

    auto keyTime = scalingKey.getTime();
    auto nextKeyTime = nextScalingKey.getTime();

    auto deltaTime = nextKeyTime - keyTime;
    auto factor = (animationTime - keyTime) / deltaTime;

#ifdef mkAssert
    assert(factor >= 0.0f && factor <= 1.0f);
#endif

    const vec3 &start = scalingKey.getValue();
    const vec3 &end = nextScalingKey.getValue();
    vec3 delta = end - start;

    out = start + factor * delta;
}

inline const AnimNode* findNodeAnim(const Animation *animation, const string &nodeName) {
    for (const auto &channel : animation->getChannels()) {
        if (channel.getName() == nodeName) {
            return &channel;
        }
    }

    return nullptr;
}

void Animator::readNodeHierarchy(float animationTime, const Node &node, const mat4 &parentTransform) {
    const auto &shape = m_model->getShape();

    const string &nodeName = node.getName();
    const Animation &animation = shape->getAnimation(m_animationIndex);
    const AnimNode *animNode = findNodeAnim(&animation, nodeName);
    mat4 nodeTransformation = node.getTransform();

    if (animNode) {
        // Интерполируем масштабирование и генерируем матрицу преобразования масштаба
        vec3 scaling;
        calcInterpolatedScaling(scaling, animationTime, animNode);

        mat4 scalingM = scale(mat4(1.0f), scaling);

        // Интерполируем вращение и генерируем матрицу вращения
        quat rotationQ;
        calcInterpolatedRotation(rotationQ, animationTime, animNode);

        mat4 rotationM = toMat4(rotationQ);

        //  Интерполируем смещение и генерируем матрицу смещения
        vec3 translation;
        calcInterpolatedPosition(translation, animationTime, animNode);

        mat4 translationM = translate(mat4(1.0f), translation);

        // Объединяем преобразования
        nodeTransformation = translationM * rotationM * scalingM;
    }

    mat4 globalTransformation = parentTransform * nodeTransformation;

    if (uint index = shape->m_bones.getIndex(nodeName); index != BonesStorage::BoneNotFound) {
        auto &bone = shape->m_bones[index];
        auto &boneTransformation = m_model->m_boneTransformations[index];
        auto &dstBone = m_model->m_animBones[m_animationIndex][index];

        globalTransformation *= boneTransformation;
        dstBone = shape->getGlobalInverseTransform() * globalTransformation * bone.getMatrix();
    }

    for (const auto &child : node.getChildren()) {
        readNodeHierarchy(animationTime, child, globalTransformation);
    }
}
}
