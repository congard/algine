#include <algine/std/model/Model.h>
#include <algine/std/model/Shape.h>
#include <algine/core/TypeRegistry.h>

using namespace std;
using namespace glm;

namespace algine {
STATIC_INITIALIZER_IMPL(Model) {
    alRegisterType(Model);
}

Model::Model(Shape *shape, Rotator::Type rotatorType, Object *parent)
    : Model(rotatorType, parent)
{
    if (shape) {
        setShape(shape);
    }
}

Model::Model(Rotator::Type rotatorType, Object *parent)
    : Object(parent),
      Rotatable(rotatorType),
      m_transform(1.0f) {}

Model::Model(Object *parent)
    : Object(parent), m_transform(1.0f) {}

Model::~Model() {
    deletePtr(m_animator)
}

void Model::transform() {
    m_transform = m_translation * m_rotation * m_scaling;
}

#define configureAnimationList() m_animBones.resize(m_shape->getAnimationsAmount())

void Model::activateAnimations() {
    configureAnimationList();

    for (auto &m_animBone : m_animBones) {
        m_animBone.resize(m_shape->getBonesAmount());
    }
}

void Model::activateAnimation(Index index) {
    configureAnimationList();
    m_animBones[index].resize(m_shape->getBonesAmount());
}

void Model::activateAnimation(std::string_view name) {
    activateAnimation(m_shape->getAnimationIndexByName(name));
}

void Model::deactivateAnimation(Index index) {
    configureAnimationList();
    m_animBones[index] = {};
}

void Model::deactivateAnimation(std::string_view name) {
    deactivateAnimation(m_shape->getAnimationIndexByName(name));
}

bool Model::isAnimationActivated(uint index) const {
    return m_animBones[index].size() == m_shape->getBonesAmount();
}

bool Model::isBonesPresent() const {
    return m_shape->isBonesPresent();
}

void Model::setBoneTransform(string_view boneName, const BoneMatrix &transformation) {
    if (uint index = m_shape->m_bones.getIndex(boneName); index != BonesStorage::BoneNotFound) {
        m_boneTransformations[index] = transformation;
    } else {
        throw runtime_error("Bone " + string(boneName) + " does not found");
    }
}

void Model::setBoneTransform(Index index, const BoneMatrix &transformation) {
    if (index < m_boneTransformations.size()) {
        m_boneTransformations[index] = transformation;
    } else {
        throw runtime_error("index >= m_boneTransformations.size()");
    }
}

void Model::setShape(Shape *shape) {
    m_shape = shape;

    configureAnimationList();

    if (m_shape->isAnimationsPresent()) {
        if (m_animator == nullptr) {
            m_animator = new Animator(this);
        }

        // nullptr check because m_bones could be already set
        if (m_bones == nullptr) {
            m_bones = &m_animBones[0];
        }
    }

    // configure transformations array
    m_boneTransformations.resize(m_shape->getBonesAmount(), mat4(1.0f));
}

void Model::setBones(const BoneMatrices *bones) {
    m_bones = bones;
}

void Model::setBonesFromAnimation(Index animationIndex) {
    m_bones = &m_animBones[animationIndex];
}

void Model::setBonesFromAnimation(string_view animationName) {
    setBonesFromAnimation(m_shape->getAnimationIndexByName(animationName));
}

void Model::setBoneTransformations(const BoneMatrices &transformations) {
    m_boneTransformations = transformations;
}

Shape* Model::getShape() const {
    return m_shape;
}

Animator* Model::getAnimator() const {
    return m_animator;
}

mat4& Model::transformation() {
    return m_transform;
}

const BoneMatrices* Model::getBones() const {
    return m_bones;
}

const BoneMatrix& Model::getBone(Index index) const {
    return m_bones->operator[](index);
}

const BoneMatrices& Model::getBoneTransformations() const {
    return m_boneTransformations;
}
}
