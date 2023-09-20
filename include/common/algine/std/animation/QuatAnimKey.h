#ifndef ALGINE_QUATANIMKEY_H
#define ALGINE_QUATANIMKEY_H

#include <algine/platform.h>
#include <glm/gtc/quaternion.hpp>

class aiQuatKey;

namespace algine {
class AL_EXPORT QuatAnimKey {
public:
    explicit QuatAnimKey(const aiQuatKey *key);

    inline double getTime64() const;
    inline float getTime() const;
    inline const glm::quat& getValue() const;

private:
    double m_time;
    glm::quat m_value;
};

double QuatAnimKey::getTime64() const {
    return m_time;
}

float QuatAnimKey::getTime() const {
    return static_cast<float>(m_time);
}

const glm::quat& QuatAnimKey::getValue() const {
    return m_value;
}
}

#endif //ALGINE_QUATANIMKEY_H
