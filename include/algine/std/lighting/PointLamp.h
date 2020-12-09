#ifndef ALGINE_POINTLAMP_H
#define ALGINE_POINTLAMP_H

#include <algine/std/lighting/PointLight.h>
#include <algine/std/model/ModelPtr.h>

namespace algine {
class PointLamp: public PointLight {
public:
    ModelPtr mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};
}

#endif //ALGINE_POINTLAMP_H
