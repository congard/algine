#ifndef ALGINE_ANIMATIONBLENDER_H
#define ALGINE_ANIMATIONBLENDER_H

#include <algine/std/model/ModelPtr.h>
#include <algine/types.h>

#include <glm/mat4x4.hpp>

#include <vector>

namespace algine {
class AnimationBlender {
public:
    enum class BlendListMode {
        Disable,
        Exclude,
        Include
    };

public:
    explicit AnimationBlender(const ModelPtr &model);
    AnimationBlender();

    void blend();
    void initInverseBoneMatrices();

    void addBlendListItem(uint item);
    void setBlendListMode(BlendListMode mode);
    void setBlendList(const std::vector<uint> &blendList);
    void setFactor(float factor);
    void changeFactor(float step);
    void setModel(const ModelPtr &model);
    void setLhsAnim(uint index);
    void setRhsAnim(uint index);

    std::vector<uint> getBlendList() const;
    BlendListMode getBlendListMode() const;
    uint getLhsAnim() const;
    uint getRhsAnim() const;
    float getFactor() const;
    const ModelPtr& getModel() const;
    const std::vector<glm::mat4>& bones();

private:
    glm::mat4 blendBones(uint index) const;

private:
    std::vector<uint> m_blendList;
    std::vector<glm::mat4> m_bones;
    std::vector<glm::mat4> m_inverseBoneMatrices;
    glm::mat4 m_inverseGlobalInverseTransform;
    BlendListMode m_blendListMode = BlendListMode::Disable;
    uint m_lhsAnim = 0, m_rhsAnim = 0;
    ModelPtr m_model = nullptr;
    float m_factor = 0.0f;
};
}

#endif //ALGINE_ANIMATIONBLENDER_H
