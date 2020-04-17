#ifndef ALGINE_DIRLAMP_H
#define ALGINE_DIRLAMP_H

#include <algine/lighting/DirLight.h>
#include <algine/model.h>

namespace algine {
namespace Lighting {
class DirLamp: public DirLight {
public:
    Model *mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};
}
}

#endif //ALGINE_DIRLAMP_H
