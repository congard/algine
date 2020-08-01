#ifndef ALGINE_POINTLAMP_H
#define ALGINE_POINTLAMP_H

#include <algine/std/lighting/PointLight.h>
#include <algine/std/model/Model.h>

namespace algine {
class PointLamp: public PointLight {
public:
    Model *mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};
}

#endif //ALGINE_POINTLAMP_H
