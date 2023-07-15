#include <algine/std/model/ModelBuilder.h>
#include <algine/std/model/Model.h>
#include <algine/core/TypeRegistry.h>

#include <tuple>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace glm;

namespace algine {
namespace Default {
constexpr auto ClassName = "Model";
}

ModelBuilder::AnimationInfo::AnimationInfo(std::string name)
    : m_name(std::move(name)),
      m_index(None) {}

ModelBuilder::AnimationInfo::AnimationInfo(Index index)
    : m_index(index) {}

ModelBuilder::AnimationInfo::AnimationInfo()
    : m_index(None) {}

const std::string& ModelBuilder::AnimationInfo::getName() const {
    return m_name;
}

Index ModelBuilder::AnimationInfo::getIndex() const {
    return m_index;
}

bool ModelBuilder::AnimationInfo::hasName() const {
    return !m_name.empty();
}

bool ModelBuilder::AnimationInfo::hasIndex() const {
    return m_index != None;
}

bool ModelBuilder::AnimationInfo::operator<(const AnimationInfo &rhs) const {
    return tie(m_name, m_index) < tie(rhs.m_name, rhs.m_index);
}

ModelBuilder::ModelBuilder()
    : m_className(Default::ClassName),
      m_rotatorType(Rotator::Type::Euler),
      m_pos(),
      m_rotate(),
      m_scale(1.0f) {}

void ModelBuilder::activateAnimation(const string &name) {
    if (m_activatedAnimations.find(AnimationInfo(AnimationInfo::All)) != m_activatedAnimations.end())
        m_activatedAnimations.clear();
    m_activatedAnimations.insert(AnimationInfo(name));
}

void ModelBuilder::activateAnimation(Index index) {
    if (index == AnimationInfo::All || m_activatedAnimations.find(AnimationInfo(AnimationInfo::All)) != m_activatedAnimations.end())
        m_activatedAnimations.clear();
    m_activatedAnimations.insert(AnimationInfo(index));
}

void ModelBuilder::setClassName(const string &name) {
    m_className = name;
}

void ModelBuilder::setShapeName(const string &name) {
    m_shapeName = name;
}

void ModelBuilder::setShape(const ShapeBuilder &builder) {
    m_shape = builder;
}

void ModelBuilder::setActiveAnimationName(const string &name) {
    m_activeAnimation = AnimationInfo(name);
}

void ModelBuilder::setActiveAnimationIndex(Index index) {
    m_activeAnimation = AnimationInfo(index);
}

void ModelBuilder::setActivatedAnimations(const set<AnimationInfo> &animations) {
    m_activatedAnimations = animations;
}

void ModelBuilder::setRotatorType(Rotator::Type type) {
    m_rotatorType = type;
}

void ModelBuilder::setPos(const vec3 &pos) {
    m_pos = pos;
}

void ModelBuilder::setRotate(const vec3 &rotate) {
    m_rotate = rotate;
}

void ModelBuilder::setScale(const vec3 &scale) {
    m_scale = scale;
}

const string& ModelBuilder::getClassName() const {
    return m_className;
}

const string &ModelBuilder::getShapeName() const {
    return m_shapeName;
}

const ShapeBuilder& ModelBuilder::getShape() const {
    return m_shape;
}

const ModelBuilder::AnimationInfo& ModelBuilder::getActiveAnimation() const {
    return m_activeAnimation;
}

const set<ModelBuilder::AnimationInfo>& ModelBuilder::getActivatedAnimations() const {
    return m_activatedAnimations;
}

Rotator::Type ModelBuilder::getRotatorType() const {
    return m_rotatorType;
}

const vec3& ModelBuilder::getPos() const {
    return m_pos;
}

const vec3& ModelBuilder::getRotate() const {
    return m_rotate;
}

const vec3& ModelBuilder::getScale() const {
    return m_scale;
}

ModelPtr ModelBuilder::get() {
    return internal::PublicObjectTools::getPtr<ModelPtr>(this);
}

ModelPtr ModelBuilder::create() {
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

void ModelBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<ModelBuilder>(s, path, lua, tenv);
}
}
