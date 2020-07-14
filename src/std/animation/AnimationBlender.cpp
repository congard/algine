#define GLM_FORCE_CTOR_INIT
#include <algine/std/animation/AnimationBlender.h>

#include <stdexcept>

using namespace std;
using namespace glm;

namespace algine {
AnimationBlender::AnimationBlender(Shape *const shape) {
    setShape(shape);
}

AnimationBlender::AnimationBlender() = default;

mat4 AnimationBlender::blendBones(uint index) const {
    const mat4 &lhs = m_shape->animations[m_lhsAnim].bones[index];
    const mat4 &rhs = m_shape->animations[m_rhsAnim].bones[index];

    const mat4 &boneMatrix = m_shape->bonesStorage[index].boneMatrix;
    const mat4 &inverseBoneMatrix = m_inverseBoneMatrices[index];

    // extracting bone transformation
    mat4 lhsTransform = inverseGlobalInverseTransform * lhs * inverseBoneMatrix;
    mat4 rhsTransform = inverseGlobalInverseTransform * rhs * inverseBoneMatrix;

    // blending lhsTransform and rhsTransform
    quat lhsQuat = quat_cast(lhsTransform);
    quat rhsQuat = quat_cast(rhsTransform);
    quat finalQuat = slerp(lhsQuat, rhsQuat, m_factor);

    vec4 lhsTranslate = lhsTransform[3];
    vec4 rhsTranslate = rhsTransform[3];
    vec4 finalTranslate = mix(lhsTranslate, rhsTranslate, m_factor);

    mat4 transform = mat4_cast(finalQuat);
    transform[3] = finalTranslate;

    // forming result
    return m_shape->globalInverseTransform * transform * boneMatrix;
}

void AnimationBlender::blend() {
    switch (m_blendListMode) {
        case BlendListDisable:
            for (uint i = 0; i < m_bones.size(); i++)
                m_bones[i] = blendBones(i);
            break;
        case BlendListInclude:
        case BlendListExclude: {
            bool isInclude = m_blendListMode == BlendListInclude;

            for (uint i = 0; i < m_bones.size(); ++i) {
                bool include = !isInclude;

                for (uint j : m_blendList) {
                    if (i == j) {
                        include = isInclude;
                        break;
                    }
                }

                if (include) {
                    m_bones[i] = blendBones(i);
                } else {
                    m_bones[i] = m_shape->animations[m_lhsAnim].bones[i];
                }
            }
            break;
        }
        default:
            throw invalid_argument(
                    "Unknown m_blendListMode value: " + to_string(m_blendListMode) + "\n"
                    "Valid values:\n"
                    "  0. AnimationBlender::BlendListDisable\n"
                    "  1. AnimationBlender::BlendListExclude\n"
                    "  2. AnimationBlender::BlendListInclude\n");
    }
}

void AnimationBlender::initInverseBoneMatrices() {
    m_inverseBoneMatrices.resize(m_shape->bonesStorage.count());

    for (uint i = 0; i < m_inverseBoneMatrices.size(); ++i)
        m_inverseBoneMatrices[i] = inverse(m_shape->bonesStorage[i].boneMatrix);
}

void AnimationBlender::addBlendListItem(const uint item) {
    m_blendList.emplace_back(item);
}

void AnimationBlender::setBlendListMode(const uint mode) {
    m_blendListMode = mode;
}

void AnimationBlender::setBlendList(const vector<uint> &blendList) {
    m_blendList = blendList;
}

inline float checkBounds(const float p, const float lowerBound, const float upperBound) {
    return (p >= lowerBound && p <= upperBound) ? p : ((p < lowerBound) ? lowerBound : upperBound);
}

inline float checkFactorBounds(const float factor) {
    return checkBounds(factor, 0.0f, 1.0f);
}

void AnimationBlender::setFactor(const float factor) {
    m_factor = checkFactorBounds(factor);
}

void AnimationBlender::changeFactor(const float step) {
    m_factor = checkFactorBounds(m_factor + step);
}

void AnimationBlender::setShape(Shape *const shape) {
    m_shape = shape;
    m_bones.resize(m_shape->bonesStorage.count());
    inverseGlobalInverseTransform = inverse(m_shape->globalInverseTransform);
    initInverseBoneMatrices();
}

void AnimationBlender::setLhsAnim(const uint index) {
    m_lhsAnim = index;
}

void AnimationBlender::setRhsAnim(const uint index) {
    m_rhsAnim = index;
}

vector<uint> AnimationBlender::getBlendList() const {
    return m_blendList;
}

uint AnimationBlender::getBlendListMode() const {
    return m_blendListMode;
}

uint AnimationBlender::getLhsAnim() const {
    return m_lhsAnim;
}

uint AnimationBlender::getRhsAnim() const {
    return m_rhsAnim;
}

float AnimationBlender::getFactor() const {
    return m_factor;
}

Shape* AnimationBlender::getShape() const {
    return m_shape;
}

const vector<mat4>& AnimationBlender::bones() {
    return m_bones;
}
}
