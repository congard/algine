#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/Model.h>

#include "../../core/PublicObjectTools.h"

using namespace std;
using namespace glm;

namespace algine {
vector<ModelPtr> Model::publicObjects;

Model::Model(const ShapePtr &shape, Rotator::Type rotatorType)
    : Rotatable(rotatorType)
{
    if (shape) {
        setShape(shape);
    }
}

Model::Model(Rotator::Type rotatorType)
    : Rotatable(rotatorType)
{
    // see initializer list above
}

Model::~Model() {
    deletePtr(m_animator)
}

void Model::transform() {
    m_transform = m_translation * m_rotation * m_scaling;
}

void Model::setShape(const ShapePtr &shape) {
    m_shape = shape;

    if (!m_shape->animations.empty()) {
        if (m_animator == nullptr) {
            m_animator = new Animator(m_shape);
        } else {
            // suddenly setShape function is called one more time
            m_animator->setShape(m_shape);
        }

        // nullptr check because m_bones could be already set
        if (m_bones == nullptr) {
            m_bones = &m_shape->animations[0].bones;
        }
    }
}

void Model::setBones(const vector<mat4> *bones) {
    m_bones = bones;
}

void Model::setBonesFromAnimation(Index animationIndex) {
    m_bones = &m_shape->animations[animationIndex].bones;
}

void Model::setBonesFromAnimation(const string &animationName) {
    setBonesFromAnimation(m_shape->getAnimationIndexByName(animationName));
}

const ShapePtr& Model::getShape() const {
    return m_shape;
}

Animator* Model::getAnimator() const {
    return m_animator;
}

mat4& Model::transformation() {
    return m_transform;
}

const vector<mat4>* Model::getBones() const {
    return m_bones;
}

const mat4& Model::getBone(Index index) const {
    return m_bones->operator[](index);
}

ModelPtr Model::getByName(const string &name) {
    return PublicObjectTools::getByName<ModelPtr>(name);
}

Model* Model::byName(const string &name) {
    return PublicObjectTools::byName<Model>(name);
}
}
