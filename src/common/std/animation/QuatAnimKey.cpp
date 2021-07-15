#include <algine/std/animation/QuatAnimKey.h>
#include <assimp/anim.h>

namespace algine {
QuatAnimKey::QuatAnimKey(const aiQuatKey *key)
    : m_time(key->mTime),
      m_value(key->mValue.w, key->mValue.x, key->mValue.y, key->mValue.z) {}
}
