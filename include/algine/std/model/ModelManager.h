#ifndef ALGINE_MODELMANAGER_H
#define ALGINE_MODELMANAGER_H

#include <algine/std/model/ShapeManager.h>
#include <algine/std/model/ModelPtr.h>
#include <algine/std/model/Model.h>
#include <algine/std/rotator/Rotator.h>

#include "algine/internal/PublicObjectTools.h"

namespace algine {
class ModelManager: public ManagerBase {
public:
    enum class ShapeDumpMode {
        None,
        Path,
        Dump,
        Name
    };

    enum class ActiveAnimationDumpMode {
        None,
        Index,
        Name
    };

public:
    ModelManager();

    void setShapeDumpMode(ShapeDumpMode mode);
    void setActiveAnimationDumpMode(ActiveAnimationDumpMode mode);

    void setShapePath(const std::string &path);
    void setShapeName(const std::string &name);
    void setShape(const ShapeManager &manager);

    void setActiveAnimationName(const std::string &name);
    void setActiveAnimationIndex(Index index);

    void setRotatorType(Rotator::Type type);
    void setPos(const glm::vec3 &pos);
    void setRotate(const glm::vec3 &rotate);
    void setScale(const glm::vec3 &scale);

    ShapeDumpMode getShapeDumpMode() const;
    ActiveAnimationDumpMode getActiveAnimationDumpMode() const;

    const std::string& getShapePath() const;
    const std::string& getShapeName() const;
    const ShapeManager& getShape() const;

    const std::string& getActiveAnimationName() const;
    Index getActiveAnimationIndex() const;

    Rotator::Type getRotatorType() const;
    const glm::vec3& getPos() const;
    const glm::vec3& getRotate() const;
    const glm::vec3& getScale() const;

    template<typename TModel = Model, typename TShape = Shape>
    ModelPtr get();

    template<typename TModel = Model, typename TShape = Shape>
    ModelPtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    ShapeDumpMode m_shapeDumpMode;
    ActiveAnimationDumpMode m_activeAnimationDumpMode;

private:
    std::string m_shapePath, m_shapeName;
    ShapeManager m_shape;

    std::string m_animationName;
    Index m_animationIndex;

    Rotator::Type m_rotatorType;
    glm::vec3 m_pos, m_rotate, m_scale;
};

template<typename TModel, typename TShape>
ModelPtr ModelManager::get() {
    return internal::PublicObjectTools::getPtr<ModelPtr, TModel, TShape>(this);
}

template<typename TModel, typename TShape>
ModelPtr ModelManager::create() {
    ModelPtr model = PtrMaker::make<TModel>(m_rotatorType);

    // create model & add Shape
    switch (m_shapeDumpMode) {
        case ShapeDumpMode::Path: {
            ShapeManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.importFromFile(m_shapePath);

            model->setShape(manager.get<TShape>());

            break;
        }
        case ShapeDumpMode::Dump: {
            model->setShape(m_shape.get<TShape>());
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

    // configure animation
    switch (m_activeAnimationDumpMode) {
        case ActiveAnimationDumpMode::Index: {
            model->setBonesFromAnimation(m_animationIndex);
            break;
        }
        case ActiveAnimationDumpMode::Name: {
            model->setBonesFromAnimation(m_animationName);
            break;
        }
        default: {
            // Explicitly left empty
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
}

#endif //ALGINE_MODELMANAGER_H
