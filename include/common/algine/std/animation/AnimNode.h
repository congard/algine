#ifndef ALGINE_ANIMNODE_H
#define ALGINE_ANIMNODE_H

#include <algine/std/animation/VecAnimKey.h>
#include <algine/std/animation/QuatAnimKey.h>
#include <algine/platform.h>
#include <algine/types.h>

#include <string>
#include <vector>

class aiNodeAnim;

namespace algine {
class AL_EXPORT AnimNode {
public:
    explicit AnimNode(const aiNodeAnim *nodeAnim);

    inline const std::string& getName() const;
    inline const std::vector<VecAnimKey>& getScalingKeys() const;
    inline const std::vector<VecAnimKey>& getPositionKeys() const;
    inline const std::vector<QuatAnimKey>& getRotationKeys() const;

    inline const VecAnimKey& getScalingKey(Index index) const;
    inline const VecAnimKey& getPositionKey(Index index) const;
    inline const QuatAnimKey& getRotationKey(Index index) const;

    inline uint scalingKeysCount() const;
    inline uint positionKeysCount() const;
    inline uint rotationKeysCount() const;

private:
    std::string m_name;
    std::vector<VecAnimKey> m_scalingKeys;
    std::vector<VecAnimKey> m_positionKeys;
    std::vector<QuatAnimKey> m_rotationKeys;
};

const std::string& AnimNode::getName() const {
    return m_name;
}

const std::vector<VecAnimKey>& AnimNode::getScalingKeys() const {
    return m_scalingKeys;
}

const std::vector<VecAnimKey>& AnimNode::getPositionKeys() const {
    return m_positionKeys;
}

const std::vector<QuatAnimKey>& AnimNode::getRotationKeys() const {
    return m_rotationKeys;
}

const VecAnimKey& AnimNode::getScalingKey(Index index) const {
    return m_scalingKeys[index];
}

const VecAnimKey& AnimNode::getPositionKey(Index index) const {
    return m_positionKeys[index];
}

const QuatAnimKey& AnimNode::getRotationKey(Index index) const {
    return m_rotationKeys[index];
}

uint AnimNode::scalingKeysCount() const {
    return m_scalingKeys.size();
}

uint AnimNode::positionKeysCount() const {
    return m_positionKeys.size();
}

uint AnimNode::rotationKeysCount() const {
    return m_rotationKeys.size();
}
}

#endif //ALGINE_ANIMNODE_H
