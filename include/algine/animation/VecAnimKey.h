#ifndef ALGINE_VECANIMKEY_H
#define ALGINE_VECANIMKEY_H

#include <glm/vec3.hpp>

class aiVectorKey;

namespace algine {
class VecAnimKey {
public:
    explicit VecAnimKey(const aiVectorKey *key);

    float getTime() const;

public:
    double time;
    glm::vec3 value;
};
}

#endif //ALGINE_VECANIMKEY_H
