#ifndef ALGINE_POINTLAMP_H
#define ALGINE_POINTLAMP_H

#include <algine/lighting/PointLight.h>
#include <algine/model.h>

namespace algine {
namespace Lighting {
class PointLamp: public PointLight {
public:
    Model *mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};
}
}

#endif //ALGINE_POINTLAMP_H
