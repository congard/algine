#ifndef ALGINE_EULERROTATOR_H
#define ALGINE_EULERROTATOR_H

#include <algine/std/rotator/Rotator.h>

namespace algine {
class EulerRotator: public Rotator {
public:
    EulerRotator();

    void rotate(glm::mat4 &matrix) override;
};
}

#endif //ALGINE_EULERROTATOR_H
