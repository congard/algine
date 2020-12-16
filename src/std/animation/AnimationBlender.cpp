#define GLM_FORCE_CTOR_INIT
#include <algine/std/animation/AnimationBlender.h>

#include <algine/std/model/Model.h>
#include <algine/std/model/Shape.h>

#include <stdexcept>

using namespace std;
using namespace glm;

namespace algine {
AnimationBlender::AnimationBlender(const ModelPtr &model) {
    setModel(model);
}

AnimationBlender::AnimationBlender() = default;

mat4 AnimationBlender::blendBones(uint index) const {
    const auto &shape = m_model->getShape();

    const mat4 &lhs = m_model->m_animBones[m_lhsAnim][index];
    const mat4 &rhs = m_model->m_animBones[m_rhsAnim][index];

    const mat4 &boneMatrix = shape->m_bones[index].boneMatrix;
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
    return shape->getGlobalInverseTransform() * transform * boneMatrix;
}

void AnimationBlender::blend() {
    switch (m_blendListMode) {
        case BlendListDisable: {
            for (uint i = 0; i < m_bones.size(); i++) {
                m_bones[i] = blendBones(i);
            }

            break;
        }
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
                    m_bones[i] = m_model->m_animBones[m_lhsAnim][i];
                }
            }
            break;
        }
        default: {
            throw invalid_argument(
                "Unknown m_blendListMode value: " + to_string(m_blendListMode) + "\n"
                "Valid values:\n"
                "  0. AnimationBlender::BlendListDisable\n"
                "  1. AnimationBlender::BlendListExclude\n"
                "  2. AnimationBlender::BlendListInclude\n"
            );
        }
    }
}

void AnimationBlender::initInverseBoneMatrices() {
    const auto &shape = m_model->getShape();

    m_inverseBoneMatrices.resize(shape->getBonesAmount());

    for (uint i = 0; i < m_inverseBoneMatrices.size(); ++i) {
        m_inverseBoneMatrices[i] = inverse(shape->m_bones[i].boneMatrix);
    }
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

void AnimationBlender::setModel(const ModelPtr &model) {
    const auto &shape = model->getShape();

    m_model = model;
    m_bones.resize(shape->getBonesAmount());
    inverseGlobalInverseTransform = inverse(shape->getGlobalInverseTransform());

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

const ModelPtr& AnimationBlender::getModel() const {
    return m_model;
}

const vector<mat4>& AnimationBlender::bones() {
    return m_bones;
}
}
