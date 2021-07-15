#include <algine/std/animation/AnimNode.h>
#include <algine/types.h>

#include <assimp/anim.h>

namespace algine {
AnimNode::AnimNode(const aiNodeAnim *nodeAnim)
    : m_name(nodeAnim->mNodeName.data)
{
    // allocating space for scaling, position, rotation keys
    m_scalingKeys.reserve(nodeAnim->mNumScalingKeys);
    m_positionKeys.reserve(nodeAnim->mNumPositionKeys);
    m_rotationKeys.reserve(nodeAnim->mNumRotationKeys);

    // filling arrays
    for (uint i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        m_scalingKeys.emplace_back(&nodeAnim->mScalingKeys[i]);
    }

    for (uint i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        m_positionKeys.emplace_back(&nodeAnim->mPositionKeys[i]);
    }

    for (uint i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        m_rotationKeys.emplace_back(&nodeAnim->mRotationKeys[i]);
    }
}
}