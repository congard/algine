#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/ModelManager.h>

#include <algine/std/model/Model.h>

#include <algine/core/transfer/GLMTransferrer.h>
#include <algine/core/PtrMaker.h>

#include "../../core/ConfigStrings.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace glm;
using namespace nlohmann;

namespace algine {
namespace Config {
constant(Shape, "shape");
constant(Rotator, "rotator");
constant(ActiveAnimation, "activeAnimation");

constant(Yaw, "yaw");
constant(Pitch, "pitch");
constant(Roll, "roll");

constant(Simple, "simple");
constant(Euler, "euler");
constant(Free, "free");

constant(Pos, "pos");
constant(Scale, "scale");
}

ModelManager::ModelManager()
    : m_shapeDumpMode(ShapeDumpMode::None),
      m_activeAnimationDumpMode(ActiveAnimationDumpMode::None),
      m_animationIndex(0),
      m_rotatorType(Rotator::Type::Euler),
      m_pos(),
      m_rotate(),
      m_scale(1.0f)
{
    // see initializer list above
}

void ModelManager::setShapeDumpMode(ShapeDumpMode mode) {
    m_shapeDumpMode = mode;
}

void ModelManager::setActiveAnimationDumpMode(ActiveAnimationDumpMode mode) {
    m_activeAnimationDumpMode = mode;
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
    setActiveAnimationDumpMode(ActiveAnimationDumpMode::Name);

    m_animationName = name;
}

void ModelManager::setActiveAnimationIndex(Index index) {
    setActiveAnimationDumpMode(ActiveAnimationDumpMode::Index);

    m_animationIndex = index;
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

ModelManager::ShapeDumpMode ModelManager::getShapeDumpMode() const {
    return m_shapeDumpMode;
}

ModelManager::ActiveAnimationDumpMode ModelManager::getActiveAnimationDumpMode() const {
    return m_activeAnimationDumpMode;
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

const string& ModelManager::getActiveAnimationName() const {
    return m_animationName;
}

Index ModelManager::getActiveAnimationIndex() const {
    return m_animationIndex;
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

void ModelManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

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
    switch (m_activeAnimationDumpMode) {
        case ActiveAnimationDumpMode::Index: {
            config[Config::ActiveAnimation] = m_animationIndex;
            break;
        }
        case ActiveAnimationDumpMode::Name: {
            config[Config::ActiveAnimation] = m_animationName;
            break;
        }
        default: {
            // empty
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
