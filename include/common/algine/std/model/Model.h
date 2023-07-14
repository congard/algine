#ifndef ALGINE_MODEL_H
#define ALGINE_MODEL_H

#include <algine/std/animation/Animator.h>
#include <algine/std/animation/BoneMatrices.h>
#include <algine/std/Translatable.h>
#include <algine/std/Scalable.h>
#include <algine/std/Rotatable.h>
#include <algine/std/model/ShapePtr.h>
#include <algine/std/model/ModelPtr.h>

#include <algine/core/Object.h>

#include <tulz/static_initializer.h>

namespace algine {
class Model: public Object, public Rotatable, public Translatable, public Scalable {
    friend class Animator;
    friend class AnimationBlender;

    STATIC_INITIALIZER_DECL

public:
    Model(const ShapePtr &shape, Rotator::Type rotatorType);
    Model();
    explicit Model(Rotator::Type rotatorType);
    virtual ~Model();

    void transform();

    void activateAnimations();
    void activateAnimation(Index index);
    void activateAnimation(std::string_view name);
    void deactivateAnimation(Index index);
    void deactivateAnimation(std::string_view name);

    bool isAnimationActivated(uint index) const;
    bool isBonesPresent() const;

    void setBoneTransform(std::string_view boneName, const BoneMatrix &transformation);
    void setBoneTransform(Index index, const BoneMatrix &transformation);

    void setShape(const ShapePtr &shape);
    void setBones(const BoneMatrices *bones);
    void setBonesFromAnimation(Index animationIndex);
    void setBonesFromAnimation(std::string_view animationName);
    void setBoneTransformations(const BoneMatrices &transformations);

    const ShapePtr& getShape() const;
    Animator* getAnimator() const;
    glm::mat4& transformation();
    const BoneMatrices* getBones() const;
    const BoneMatrix& getBone(Index index) const;
    const BoneMatrices& getBoneTransformations() const;

public:
    static ModelPtr getByName(const std::string &name);
    static Model* byName(const std::string &name);

public:
    static std::vector<ModelPtr> publicObjects;

public:
    glm::mat4 m_transform;

protected:
    ShapePtr m_shape = nullptr;
    Animator *m_animator = nullptr;
    const BoneMatrices *m_bones = nullptr;

protected:
    std::vector<BoneMatrices> m_animBones;
    BoneMatrices m_boneTransformations;
};
}

#endif // ALGINE_MODEL_H
