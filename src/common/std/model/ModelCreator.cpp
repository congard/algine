#include <algine/std/model/ModelCreator.h>

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

ModelCreator::AnimationInfo::AnimationInfo(std::string name)
    : m_name(move(name)),
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
      m_shapeDumpMode(ShapeDumpMode::None),
      m_activatedAnimationsDumpMode(ActivatedAnimationsDumpMode::List),
      m_rotatorType(Rotator::Type::Euler),
      m_pos(),
      m_rotate(),
      m_scale(1.0f) {}

void ModelCreator::activateAnimation(const string &name) {
    m_activatedAnimations.insert(AnimationInfo(name));
}

void ModelCreator::activateAnimation(Index index) {
    m_activatedAnimations.insert(AnimationInfo(index));
}

void ModelCreator::setClassName(const string &name) {
    m_className = name;
}

void ModelCreator::setShapeDumpMode(ShapeDumpMode mode) {
    m_shapeDumpMode = mode;
}

void ModelCreator::setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode mode) {
    m_activatedAnimationsDumpMode = mode;
}

void ModelCreator::setShapePath(const string &path) {
    setShapeDumpMode(ShapeDumpMode::Path);
    m_shapePath = path;
}

void ModelCreator::setShapeName(const string &name) {
    setShapeDumpMode(ShapeDumpMode::Name);
    m_shapeName = name;
}

void ModelCreator::setShape(const ShapeCreator &creator) {
    setShapeDumpMode(ShapeDumpMode::Dump);
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

ModelCreator::ShapeDumpMode ModelCreator::getShapeDumpMode() const {
    return m_shapeDumpMode;
}

ModelCreator::ActivatedAnimationsDumpMode ModelCreator::getActivatedAnimationsDumpMode() const {
    return m_activatedAnimationsDumpMode;
}

const string &ModelCreator::getShapePath() const {
    return m_shapePath;
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
    switch (m_shapeDumpMode) {
        case ShapeDumpMode::Path: {
            ShapeCreator creator;
            creator.setIOSystem(io());
            creator.setWorkingDirectory(m_workingDirectory);
            creator.importFromFile(m_shapePath);

            model->setShape(creator.get());

            break;
        }
        case ShapeDumpMode::Dump: {
            m_shape.setIOSystem(io());
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

void ModelCreator::import(const JsonHelper &jsonHelper) {
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
            ShapeCreator creator;
            creator.setWorkingDirectory(m_workingDirectory);
            creator.import(shape[Dump]);
            setShape(creator);
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

    Creator::import(jsonHelper);
}

JsonHelper ModelCreator::dump() {
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
    result.append(Creator::dump());

    return result;
}
}
