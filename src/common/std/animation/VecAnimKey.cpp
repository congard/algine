#include <algine/std/animation/VecAnimKey.h>
#include <assimp/anim.h>

namespace algine {
VecAnimKey::VecAnimKey(const aiVectorKey *key) {
    time = key->mTime;
    value = glm::vec3(key->mValue.x, key->mValue.y, key->mValue.z);
}

float VecAnimKey::getTime() const {
    return static_cast<float>(time);
}
}