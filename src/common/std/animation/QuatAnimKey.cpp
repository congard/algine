#include <algine/std/animation/QuatAnimKey.h>
#include <assimp/anim.h>

namespace algine {
QuatAnimKey::QuatAnimKey(const aiQuatKey *key) {
    time = key->mTime;
    value = glm::quat(key->mValue.w, key->mValue.x, key->mValue.y, key->mValue.z);
}

float QuatAnimKey::getTime() const {
    return static_cast<float>(time);
}
}
