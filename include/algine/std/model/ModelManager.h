#ifndef ALGINE_MODELMANAGER_H
#define ALGINE_MODELMANAGER_H

#include <algine/std/model/ShapeManager.h>
#include <algine/std/model/ModelPtr.h>
#include <algine/std/model/Model.h>
#include <algine/std/rotator/Rotator.h>

#include <algine/internal/PublicObjectTools.h>

#include <set>

namespace algine {
class ModelManager: public ManagerBase {
public:
    enum class ShapeDumpMode {
        None,
        Path,
        Dump,
        Name
    };

    enum class ActivatedAnimationsDumpMode {
        All,
        List
    };

public:
    class AnimationInfo {
    public:
        constexpr static Index None = -1;

    public:
        explicit AnimationInfo(std::string name);
        explicit AnimationInfo(Index index);
        AnimationInfo();

        const std::string& getName() const;
        Index getIndex() const;

        bool hasName() const;
        bool hasIndex() const;

        bool operator<(const AnimationInfo &rhs) const;

    private:
        std::string m_name;
        Index m_index;
    };

public:
    ModelManager();

    void activateAnimation(const std::string &name);
    void activateAnimation(Index index);

    void setShapeDumpMode(ShapeDumpMode mode);
    void setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode mode);

    void setShapePath(const std::string &path);
    void setShapeName(const std::string &name);
    void setShape(const ShapeManager &manager);

    void setActiveAnimationName(const std::string &name);
    void setActiveAnimationIndex(Index index);

    void setActivatedAnimations(const std::set<AnimationInfo> &animations);

    void setRotatorType(Rotator::Type type);
    void setPos(const glm::vec3 &pos);
    void setRotate(const glm::vec3 &rotate);
    void setScale(const glm::vec3 &scale);

    ShapeDumpMode getShapeDumpMode() const;
    ActivatedAnimationsDumpMode getActivatedAnimationsDumpMode() const;

    const std::string& getShapePath() const;
    const std::string& getShapeName() const;
    const ShapeManager& getShape() const;

    const AnimationInfo& getActiveAnimation() const;

    const std::set<AnimationInfo>& getActivatedAnimations() const;

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
    ActivatedAnimationsDumpMode m_activatedAnimationsDumpMode;

private:
    std::string m_shapePath, m_shapeName;
    ShapeManager m_shape;

    AnimationInfo m_activeAnimation;

    Rotator::Type m_rotatorType;
    glm::vec3 m_pos, m_rotate, m_scale;

    std::set<AnimationInfo> m_activatedAnimations;
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
}

#endif //ALGINE_MODELMANAGER_H
