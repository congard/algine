#include <algine/std/animation/VecAnimKey.h>
#include <assimp/anim.h>

namespace algine {
VecAnimKey::VecAnimKey(const aiVectorKey *key)
    : m_time(key->mTime),
      m_value(key->mValue.x, key->mValue.y, key->mValue.z) {}
}