#ifndef ALGINE_FREEROTATOR_H
#define ALGINE_FREEROTATOR_H

#include <algine/std/rotator/Rotator.h>

#include <glm/gtc/quaternion.hpp>

namespace algine {
class FreeRotator: public Rotator {
public:
    FreeRotator();

    void rotate(glm::mat4 &matrix) override;

public:
    glm::quat qRotation;
};
}

#endif //ALGINE_FREEROTATOR_H
