#include <algine/std/lighting/PointLamp.h>

namespace algine {
namespace Lighting {
void PointLamp::setPos(const glm::vec3 &pos) {
    Light::m_pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}
}
}
