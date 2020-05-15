#include <algine/std/lighting/DirLamp.h>

namespace algine {
namespace Lighting {
void DirLamp::setPos(const glm::vec3 &pos) {
    Light::m_pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}
}
}
