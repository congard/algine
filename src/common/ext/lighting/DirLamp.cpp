#include <algine/ext/lighting/DirLamp.h>

#include <algine/std/model/Model.h>

namespace algine {
void DirLamp::setPos(const glm::vec3 &pos) {
    Light::m_pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}
}
