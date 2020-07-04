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
    explicit Model(uint rotatorType = Rotator::RotatorTypeSimple);

    void updateMatrix();

public:
    Shape *shape = nullptr;
    Animator *animator = nullptr;
    glm::mat4 m_transform;
};
}

#endif // ALGINE_MODEL_H
