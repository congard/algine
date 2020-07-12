#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/Model.h>

using namespace std;
using namespace glm;

namespace algine {
Model::Model(const uint rotatorType): Rotatable(rotatorType) {
    /* empty */
}

void Model::updateMatrix() {
    m_transform = m_translation * m_rotation * m_scaling;
}

void Model::setShape(Shape *shape) {
    m_shape = shape;

    // nullptr check because m_bones could be already set
    if (!m_shape->animations.empty() && m_bones == nullptr)
        m_bones = &m_shape->animations[0].bones;
}

void Model::setAnimator(Animator *animator) {
    m_animator = animator;
}

void Model::setBones(vector<mat4> *bones) {
    m_bones = bones;
}

void Model::setBonesFromAnimation(const uint animationIndex) {
    m_bones = &m_shape->animations[animationIndex].bones;
}

void Model::setBonesFromAnimation(const string &animationName) {
    setBonesFromAnimation(m_shape->getAnimationIndexByName(animationName));
}

Shape* Model::getShape() const {
    return m_shape;
}

Animator* Model::getAnimator() const {
    return m_animator;
}

vector<mat4>* Model::getBones() const {
    return m_bones;
}

mat4& Model::getBone(const uint index) const {
    return m_bones->operator[](index);
}

mat4& Model::transformation() {
    return m_transform;
}
}
