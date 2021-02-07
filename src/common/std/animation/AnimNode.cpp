#include <algine/std/animation/AnimNode.h>
#include <algine/types.h>

#include <assimp/anim.h>

namespace algine {
AnimNode::AnimNode(const aiNodeAnim *nodeAnim) {
    name = nodeAnim->mNodeName.data;

    // allocating space for scaling, position, rotation keys
    scalingKeys.reserve(nodeAnim->mNumScalingKeys);
    positionKeys.reserve(nodeAnim->mNumPositionKeys);
    rotationKeys.reserve(nodeAnim->mNumRotationKeys);

    // filling arrays
    for (uint i = 0; i < nodeAnim->mNumScalingKeys; i++)
        scalingKeys.emplace_back(&nodeAnim->mScalingKeys[i]);
    for (uint i = 0; i < nodeAnim->mNumPositionKeys; i++)
        positionKeys.emplace_back(&nodeAnim->mPositionKeys[i]);
    for (uint i = 0; i < nodeAnim->mNumRotationKeys; i++)
        rotationKeys.emplace_back(&nodeAnim->mRotationKeys[i]);
}
}