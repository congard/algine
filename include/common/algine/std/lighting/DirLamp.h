#ifndef ALGINE_DIRLAMP_H
#define ALGINE_DIRLAMP_H

#include <algine/std/lighting/DirLight.h>
#include <algine/std/model/ModelPtr.h>

namespace algine {
class DirLamp: public DirLight {
public:
    ModelPtr mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};
}

#endif //ALGINE_DIRLAMP_H
