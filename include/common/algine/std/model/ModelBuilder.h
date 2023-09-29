#ifndef ALGINE_MODELBUILDER_H
#define ALGINE_MODELBUILDER_H

#include <algine/std/model/ShapeBuilder.h>
#include <algine/std/model/ModelPtr.h>
#include <algine/std/model/Model.h>
#include <algine/std/rotator/Rotator.h>

#include <set>

namespace algine {
class AL_EXPORT ModelBuilder: public Builder {
    AL_BUILDER(Model)

public:
    class AnimationInfo {
    public:
        constexpr static Index None = -1;
        constexpr static Index All = -2;

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
    ModelBuilder();

    void activateAnimation(const std::string &name);
    void activateAnimation(Index index);

    void setClassName(const std::string &name);
    void setShapeName(const std::string &name);
    void setShape(const ShapeBuilder &builder);

    const std::string& getClassName() const;
    const std::string& getShapeName() const;
    const ShapeBuilder& getShape() const;

    void setActiveAnimationName(const std::string &name);
    void setActiveAnimationIndex(Index index);
    void setActivatedAnimations(const std::set<AnimationInfo> &animations);

    const AnimationInfo& getActiveAnimation() const;
    const std::set<AnimationInfo>& getActivatedAnimations() const;

    void setRotatorType(Rotator::Type type);
    void setPos(const glm::vec3 &pos);
    void setRotate(const glm::vec3 &rotate);
    void setScale(const glm::vec3 &scale);

    Rotator::Type getRotatorType() const;
    const glm::vec3& getPos() const;
    const glm::vec3& getRotate() const;
    const glm::vec3& getScale() const;

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;

private:
    std::string m_className;

    std::string m_shapeName;
    ShapeBuilder m_shape;

    AnimationInfo m_activeAnimation;

    Rotator::Type m_rotatorType;
    glm::vec3 m_pos, m_rotate, m_scale;

    std::set<AnimationInfo> m_activatedAnimations;
};
}

#endif //ALGINE_MODELBUILDER_H
