#ifndef ALGINE_QUATANIMKEY_H
#define ALGINE_QUATANIMKEY_H

#include <glm/gtc/quaternion.hpp>

class aiQuatKey;

namespace algine {
class QuatAnimKey {
public:
    explicit QuatAnimKey(const aiQuatKey *key);

    float getTime() const;

public:
    double time;
    glm::quat value;
};
}

#endif //ALGINE_QUATANIMKEY_H
