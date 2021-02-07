#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/ModelManager.h>

#include <algine/std/model/Model.h>

#include <algine/core/transfer/GLMTransferrer.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/TypeRegistry.h>

#include <iostream>
#include <vector>
#include <tuple>

#include "internal/PublicObjectTools.h"
#include "internal/ConfigStrings.h"

using namespace std;
using namespace glm;
using namespace nlohmann;

namespace algine {
namespace Config {
constant(Shape, "shape");
constant(Rotator, "rotator");
constant(ActiveAnimation, "activeAnimation");
constant(ActivatedAnimations, "activatedAnimations");

constant(All, "all");

constant(Yaw, "yaw");
constant(Pitch, "pitch");
constant(Roll, "roll");

constant(Simple, "simple");
constant(Euler, "euler");
constant(Free, "free");

constant(Pos, "pos");
constant(Scale, "scale");
}

namespace Default {
constant(ClassName, "Model");
}

ModelManager::AnimationInfo::AnimationInfo(std::string name)
    : m_name(move(name)),
      m_index(None) {}

ModelManager::AnimationInfo::AnimationInfo(Index index)
    : m_index(index) {}

ModelManager::AnimationInfo::AnimationInfo()
    : m_index(None) {}

const std::string &ModelManager::AnimationInfo::getName() const {
    return m_name;
}

Index ModelManager::AnimationInfo::getIndex() const {
    return m_index;
}

bool ModelManager::AnimationInfo::hasName() const {
    return !m_name.empty();
}

bool ModelManager::AnimationInfo::hasIndex() const {
    return m_index != None;
}

bool ModelManager::AnimationInfo::operator<(const AnimationInfo &rhs) const {
    return tie(m_name, m_index) < tie(rhs.m_name, rhs.m_index);
}

ModelManager::ModelManager()
    : m_className(Default::ClassName),
      m_shapeDumpMode(ShapeDumpMode::None),
      m_activatedAnimationsDumpMode(ActivatedAnimationsDumpMode::List),
      m_rotatorType(Rotator::Type::Euler),
      m_pos(),
      m_rotate(),
      m_scale(1.0f) {}

void ModelManager::activateAnimation(const string &name) {
    m_activatedAnimations.insert(AnimationInfo(name));
}

void ModelManager::activateAnimation(Index index) {
    m_activatedAnimations.insert(AnimationInfo(index));
}

void ModelManager::setClassName(const string &name) {
    m_className = name;
}

void ModelManager::setShapeDumpMode(ShapeDumpMode mode) {
    m_shapeDumpMode = mode;
}

void ModelManager::setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode mode) {
    m_activatedAnimationsDumpMode = mode;
}

void ModelManager::setShapePath(const string &path) {
    setShapeDumpMode(ShapeDumpMode::Path);

    m_shapePath = path;
}

void ModelManager::setShapeName(const string &name) {
    setShapeDumpMode(ShapeDumpMode::Name);

    m_shapeName = name;
}

void ModelManager::setShape(const ShapeManager &manager) {
    setShapeDumpMode(ShapeDumpMode::Dump);

    m_shape = manager;
}

void ModelManager::setActiveAnimationName(const string &name) {
    m_activeAnimation = AnimationInfo(name);
}

void ModelManager::setActiveAnimationIndex(Index index) {
    m_activeAnimation = AnimationInfo(index);
}

void ModelManager::setActivatedAnimations(const set<AnimationInfo> &animations) {
    m_activatedAnimations = animations;
}

void ModelManager::setRotatorType(Rotator::Type type) {
    m_rotatorType = type;
}

void ModelManager::setPos(const vec3 &pos) {
    m_pos = pos;
}

void ModelManager::setRotate(const vec3 &rotate) {
    m_rotate = rotate;
}

void ModelManager::setScale(const vec3 &scale) {
    m_scale = scale;
}

const string& ModelManager::getClassName() const {
    return m_className;
}

ModelManager::ShapeDumpMode ModelManager::getShapeDumpMode() const {
    return m_shapeDumpMode;
}

ModelManager::ActivatedAnimationsDumpMode ModelManager::getActivatedAnimationsDumpMode() const {
    return m_activatedAnimationsDumpMode;
}

const string &ModelManager::getShapePath() const {
    return m_shapePath;
}

const string &ModelManager::getShapeName() const {
    return m_shapeName;
}

const ShapeManager& ModelManager::getShape() const {
    return m_shape;
}

const ModelManager::AnimationInfo& ModelManager::getActiveAnimation() const {
    return m_activeAnimation;
}

const set<ModelManager::AnimationInfo>& ModelManager::getActivatedAnimations() const {
    return m_activatedAnimations;
}

Rotator::Type ModelManager::getRotatorType() const {
    return m_rotatorType;
}

const vec3& ModelManager::getPos() const {
    return m_pos;
}

const vec3& ModelManager::getRotate() const {
    return m_rotate;
}

const vec3& ModelManager::getScale() const {
    return m_scale;
}

ModelPtr ModelManager::get() {
    return internal::PublicObjectTools::getPtr<ModelPtr>(this);
}

ModelPtr ModelManager::create() {
    ModelPtr model(TypeRegistry::create<Model>(m_className));
    model->setRotatorType(m_rotatorType);

    // create model & add Shape
    switch (m_shapeDumpMode) {
        case ShapeDumpMode::Path: {
            ShapeManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.importFromFile(m_shapePath);

            model->setShape(manager.get());

            break;
        }
        case ShapeDumpMode::Dump: {
            model->setShape(m_shape.get());
            break;
        }
        case ShapeDumpMode::Name: {
            model->setShape(Shape::getByName(m_shapeName));
            break;
        }
        case ShapeDumpMode::None: {
            // Shape is nullptr
            break;
        }
    }

    // configure animations
    if (m_activeAnimation.hasIndex()) {
        model->setBonesFromAnimation(m_activeAnimation.getIndex());
    } else if (m_activeAnimation.hasName()) {
        model->setBonesFromAnimation(m_activeAnimation.getName());
    }

    switch (m_activatedAnimationsDumpMode) {
        case ActivatedAnimationsDumpMode::All: {
            model->activateAnimations();
            break;
        }
        case ActivatedAnimationsDumpMode::List: {
            for (const auto &info : m_activatedAnimations) {
                if (info.hasIndex()) {
                    model->activateAnimation(info.getIndex());
                } else if (info.hasName()) {
                    model->activateAnimation(info.getName());
                }
            }

            break;
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

void ModelManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load class name
    if (config.contains(ClassName))
        m_className = config[ClassName];

    // load shape
    if (config.contains(Config::Shape)) {
        const json &shape = config[Config::Shape];

        if (shape.contains(Path)) {
            setShapePath(shape[Path]);
        } else if (shape.contains(Dump)) {
            ShapeManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.import(shape[Dump]);

            setShape(manager);
        } else if (shape.contains(Name)) {
            setShapeName(shape[Name]);
        } else {
            throw runtime_error("Broken config:\n" + shape.dump());
        }
    }

    // load active animation
    if (config.contains(ActiveAnimation)) {
        if (const json &anim = config[ActiveAnimation]; anim.is_number_integer()) {
            setActiveAnimationIndex(anim);
        } else if (anim.is_string()) {
            setActiveAnimationName(anim);
        } else {
            throw runtime_error("Broken config:\n" + anim.dump());
        }
    }

    // load activated animations
    if (config.contains(ActivatedAnimations)) {
        if (const json &animations = config[ActivatedAnimations]; animations.is_string() && animations == All) {
            setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode::All);
        } else if (animations.is_array()) {
            setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode::List);

            for (const auto &anim : animations) {
                if (anim.is_number()) {
                    m_activatedAnimations.insert(AnimationInfo(anim.get<Index>()));
                } else if (anim.is_string()) {
                    m_activatedAnimations.insert(AnimationInfo(anim.get<string>()));
                } else {
                    throw runtime_error("Invalid activatedAnimations element:\n" + anim.dump());
                }
            }
        } else {
            throw runtime_error("Invalid activatedAnimations value:\n" + animations.dump());
        }
    }

    // load rotator
    if (config.contains(Config::Rotator)) {
        const JsonHelper &rotator = config[Config::Rotator];

        rotator.readValue(Pitch, m_rotate.x, 0.0f);
        rotator.readValue(Yaw, m_rotate.y, 0.0f);
        rotator.readValue(Roll, m_rotate.z, 0.0f);

        if (rotator.json.contains(Type)) {
            m_rotatorType = map<string, Rotator::Type> {
                {Simple, Rotator::Type::Simple},
                {Euler, Rotator::Type::Euler},
                {Free, Rotator::Type::Free}
            } [rotator.json[Type]];
        }
    }

    // load position & scale
    m_pos = GLMTransferrer::import<vec3>(jsonHelper.readValue(Pos));
    m_scale = GLMTransferrer::import<vec3>(jsonHelper.readValue(Scale));

    ManagerBase::import(jsonHelper);
}

JsonHelper ModelManager::dump() {
    json config;

    // write class name
    if (m_className != Default::ClassName)
        config[Config::ClassName] = m_className;

    // write shape
    switch (m_shapeDumpMode) {
        case ShapeDumpMode::Path: {
            config[Config::Shape][Config::Path] = m_shapePath;
            break;
        }
        case ShapeDumpMode::Dump: {
            config[Config::Shape][Config::Dump] = m_shape.dump().get();
            break;
        }
        case ShapeDumpMode::Name: {
            config[Config::Shape][Config::Name] = m_shapeName;
            break;
        }
        default: {
            // empty
        }
    }

    // write active animation
    if (m_activeAnimation.hasIndex()) {
        config[Config::ActiveAnimation] = m_activeAnimation.getIndex();
    } else if (m_activeAnimation.hasName()) {
        config[Config::ActiveAnimation] = m_activeAnimation.getName();
    }

    // write activated animations
    switch (m_activatedAnimationsDumpMode) {
        case ActivatedAnimationsDumpMode::All: {
            config[Config::ActivatedAnimations] = Config::All;
            break;
        }
        case ActivatedAnimationsDumpMode::List: {
            for (const auto &info : m_activatedAnimations) {
                if (info.hasIndex()) {
                    config[Config::ActivatedAnimations].emplace_back(info.getIndex());
                } else if (info.hasName()) {
                    config[Config::ActivatedAnimations].emplace_back(info.getName());
                }
            }

            break;
        }
    }

    // write rotator
    {
        using namespace Config;

        json &rotator = config[Config::Rotator];
        rotator[Yaw] = m_rotate.y;
        rotator[Pitch] = m_rotate.x;
        rotator[Roll] = m_rotate.z;
        rotator[Type] = (vector<string> {Simple, Euler, Free})[static_cast<uint>(m_rotatorType)];
    }

    // write position & scale
    config[Config::Pos] = GLMTransferrer::dump(m_pos).get();
    config[Config::Scale] = GLMTransferrer::dump(m_scale).get();

    JsonHelper result(config);
    result.append(ManagerBase::dump());

    return result;
}
}
