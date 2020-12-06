#ifndef ALGINE_MODEL_H
#define ALGINE_MODEL_H

#include <algine/std/animation/Animator.h>
#include <algine/std/Translatable.h>
#include <algine/std/Scalable.h>
#include <algine/std/Rotatable.h>
#include <algine/std/model/Shape.h>
#include <algine/std/model/ModelPtr.h>

namespace algine {
class Model: public Object, public Rotatable, public Translatable, public Scalable {
public:
    explicit Model(const ShapePtr &shape = nullptr, Rotator::Type rotatorType = Rotator::Type::Simple);
    explicit Model(Rotator::Type rotatorType);
    ~Model();

    void transform();

    void setShape(const ShapePtr &shape);
    void setBones(const std::vector<glm::mat4> *bones);
    void setBonesFromAnimation(Index animationIndex);
    void setBonesFromAnimation(const std::string &animationName);

    const ShapePtr& getShape() const;
    Animator* getAnimator() const;
    glm::mat4& transformation();
    const std::vector<glm::mat4>* getBones() const;
    const glm::mat4& getBone(Index index) const;

public:
    static ModelPtr getByName(const std::string &name);
    static Model* byName(const std::string &name);

public:
    static std::vector<ModelPtr> publicObjects;

public:
    glm::mat4 m_transform;

private:
    ShapePtr m_shape = nullptr;
    Animator *m_animator = nullptr;
    const std::vector<glm::mat4> *m_bones = nullptr;
};
}

#endif // ALGINE_MODEL_H
