#ifndef ALGINE_VECANIMKEY_H
#define ALGINE_VECANIMKEY_H

#include <algine/platform.h>
#include <glm/vec3.hpp>

class aiVectorKey;

namespace algine {
class AL_EXPORT VecAnimKey {
public:
    explicit VecAnimKey(const aiVectorKey *key);

    inline double getTime64() const;
    inline float getTime() const;
    inline const glm::vec3& getValue() const;

private:
    double m_time;
    glm::vec3 m_value;
};

double VecAnimKey::getTime64() const {
    return m_time;
}

float VecAnimKey::getTime() const {
    return static_cast<float>(m_time);
}

const glm::vec3& VecAnimKey::getValue() const {
    return m_value;
}
}

#endif //ALGINE_VECANIMKEY_H
