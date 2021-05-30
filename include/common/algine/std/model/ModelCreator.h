#ifndef ALGINE_MODELCREATOR_H
#define ALGINE_MODELCREATOR_H

#include <algine/std/model/ShapeCreator.h>
#include <algine/std/model/ModelPtr.h>
#include <algine/std/model/Model.h>
#include <algine/std/rotator/Rotator.h>

#include <set>

namespace algine {
class ModelCreator: public Creator {
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
    ModelCreator();

    void activateAnimation(const std::string &name);
    void activateAnimation(Index index);

    void setClassName(const std::string &name);

    void setShapeDumpMode(ShapeDumpMode mode);
    void setActivatedAnimationsDumpMode(ActivatedAnimationsDumpMode mode);

    void setShapePath(const std::string &path);
    void setShapeName(const std::string &name);
    void setShape(const ShapeCreator &creator);

    void setActiveAnimationName(const std::string &name);
    void setActiveAnimationIndex(Index index);

    void setActivatedAnimations(const std::set<AnimationInfo> &animations);

    void setRotatorType(Rotator::Type type);
    void setPos(const glm::vec3 &pos);
    void setRotate(const glm::vec3 &rotate);
    void setScale(const glm::vec3 &scale);

    const std::string& getClassName() const;

    ShapeDumpMode getShapeDumpMode() const;
    ActivatedAnimationsDumpMode getActivatedAnimationsDumpMode() const;

    const std::string& getShapePath() const;
    const std::string& getShapeName() const;
    const ShapeCreator& getShape() const;

    const AnimationInfo& getActiveAnimation() const;

    const std::set<AnimationInfo>& getActivatedAnimations() const;

    Rotator::Type getRotatorType() const;
    const glm::vec3& getPos() const;
    const glm::vec3& getRotate() const;
    const glm::vec3& getScale() const;

    ModelPtr get();
    ModelPtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    ShapeDumpMode m_shapeDumpMode;
    ActivatedAnimationsDumpMode m_activatedAnimationsDumpMode;

private:
    std::string m_className;

    std::string m_shapePath, m_shapeName;
    ShapeCreator m_shape;

    AnimationInfo m_activeAnimation;

    Rotator::Type m_rotatorType;
    glm::vec3 m_pos, m_rotate, m_scale;

    std::set<AnimationInfo> m_activatedAnimations;
};
}

#endif //ALGINE_MODELCREATOR_H
