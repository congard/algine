#include <algine/std/model/ModelCreator.h>
#include <algine/std/model/Model.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/TypeRegistry.h>

#include <tuple>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace glm;

namespace algine {
namespace Default {
constexpr auto ClassName = "Model";
}

ModelCreator::AnimationInfo::AnimationInfo(std::string name)
    : m_name(std::move(name)),
      m_index(None) {}

ModelCreator::AnimationInfo::AnimationInfo(Index index)
    : m_index(index) {}

ModelCreator::AnimationInfo::AnimationInfo()
    : m_index(None) {}

const std::string &ModelCreator::AnimationInfo::getName() const {
    return m_name;
}

Index ModelCreator::AnimationInfo::getIndex() const {
    return m_index;
}

bool ModelCreator::AnimationInfo::hasName() const {
    return !m_name.empty();
}

bool ModelCreator::AnimationInfo::hasIndex() const {
    return m_index != None;
}

bool ModelCreator::AnimationInfo::operator<(const AnimationInfo &rhs) const {
    return tie(m_name, m_index) < tie(rhs.m_name, rhs.m_index);
}

ModelCreator::ModelCreator()
    : m_className(Default::ClassName),
      m_rotatorType(Rotator::Type::Euler),
      m_pos(),
      m_rotate(),
      m_scale(1.0f) {}

void ModelCreator::activateAnimation(const string &name) {
    if (m_activatedAnimations.find(AnimationInfo(AnimationInfo::All)) != m_activatedAnimations.end())
        m_activatedAnimations.clear();
    m_activatedAnimations.insert(AnimationInfo(name));
}

void ModelCreator::activateAnimation(Index index) {
    if (index == AnimationInfo::All || m_activatedAnimations.find(AnimationInfo(AnimationInfo::All)) != m_activatedAnimations.end())
        m_activatedAnimations.clear();
    m_activatedAnimations.insert(AnimationInfo(index));
}

void ModelCreator::setClassName(const string &name) {
    m_className = name;
}

void ModelCreator::setShapeName(const string &name) {
    m_shapeName = name;
}

void ModelCreator::setShape(const ShapeCreator &creator) {
    m_shape = creator;
}

void ModelCreator::setActiveAnimationName(const string &name) {
    m_activeAnimation = AnimationInfo(name);
}

void ModelCreator::setActiveAnimationIndex(Index index) {
    m_activeAnimation = AnimationInfo(index);
}

void ModelCreator::setActivatedAnimations(const set<AnimationInfo> &animations) {
    m_activatedAnimations = animations;
}

void ModelCreator::setRotatorType(Rotator::Type type) {
    m_rotatorType = type;
}

void ModelCreator::setPos(const vec3 &pos) {
    m_pos = pos;
}

void ModelCreator::setRotate(const vec3 &rotate) {
    m_rotate = rotate;
}

void ModelCreator::setScale(const vec3 &scale) {
    m_scale = scale;
}

const string& ModelCreator::getClassName() const {
    return m_className;
}

const string &ModelCreator::getShapeName() const {
    return m_shapeName;
}

const ShapeCreator& ModelCreator::getShape() const {
    return m_shape;
}

const ModelCreator::AnimationInfo& ModelCreator::getActiveAnimation() const {
    return m_activeAnimation;
}

const set<ModelCreator::AnimationInfo>& ModelCreator::getActivatedAnimations() const {
    return m_activatedAnimations;
}

Rotator::Type ModelCreator::getRotatorType() const {
    return m_rotatorType;
}

const vec3& ModelCreator::getPos() const {
    return m_pos;
}

const vec3& ModelCreator::getRotate() const {
    return m_rotate;
}

const vec3& ModelCreator::getScale() const {
    return m_scale;
}

ModelPtr ModelCreator::get() {
    return internal::PublicObjectTools::getPtr<ModelPtr>(this);
}

ModelPtr ModelCreator::create() {
    ModelPtr model(TypeRegistry::create<Model>(m_className));
    model->setRotatorType(m_rotatorType);

    // create model & add Shape
    if (!m_shapeName.empty()) {
        model->setShape(Shape::getByName(m_shapeName));
    } else if (!m_shape.getModelPath().empty()) {
        m_shape.setIOSystem(io());
        model->setShape(m_shape.get());
    }

    // configure animations
    if (m_activeAnimation.hasIndex()) {
        model->setBonesFromAnimation(m_activeAnimation.getIndex());
    } else if (m_activeAnimation.hasName()) {
        model->setBonesFromAnimation(m_activeAnimation.getName());
    }

    if (m_activatedAnimations.find(AnimationInfo(AnimationInfo::All)) != m_activatedAnimations.end()) {
        model->activateAnimations();
    } else {
        for (const auto &info : m_activatedAnimations) {
            if (info.hasIndex()) {
                model->activateAnimation(info.getIndex());
            } else if (info.hasName()) {
                model->activateAnimation(info.getName());
            }
        }
    }

    // set params
    model->setPos(m_pos);
    model->setRotate(m_rotate.x, m_rotate.y, m_rotate.z);
    model->setScale(m_scale);

    // init matrices
    model->translate();
    model->rotate();
    model->scale();
    model->transform();

    internal::PublicObjectTools::postCreateAccessOp("Model", this, model);

    return model;
}

void ModelCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<ModelCreator>(s, path, lua, tenv);
}
}
