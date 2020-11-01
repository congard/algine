#ifndef ALGINE_MODEL_H
#define ALGINE_MODEL_H

#include <algine/std/animation/Animator.h>
#include <algine/std/Translatable.h>
#include <algine/std/Scalable.h>
#include <algine/std/Rotatable.h>
#include <algine/std/model/Shape.h>

namespace algine {
class Model: public Rotatable, public Translatable, public Scalable {
public:
    explicit Model(Shape *shape = nullptr, Rotator::Type rotatorType = Rotator::Type::Simple);
    ~Model();

    void updateMatrix();

    void setShape(Shape *shape);
    void setBones(const std::vector<glm::mat4> *bones);
    void setBonesFromAnimation(Index animationIndex);
    void setBonesFromAnimation(const std::string &animationName);

    Shape* getShape() const;
    Animator* getAnimator() const;
    glm::mat4& transformation();
    const std::vector<glm::mat4>* getBones() const;
    const glm::mat4& getBone(Index index) const;

public:
    glm::mat4 m_transform;

private:
    Shape *m_shape = nullptr;
    Animator *m_animator = nullptr;
    const std::vector<glm::mat4> *m_bones = nullptr;
};
}

#endif // ALGINE_MODEL_H
