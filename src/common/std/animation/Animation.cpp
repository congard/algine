#include <algine/std/animation/Animation.h>

#include <algine/types.h>

#include <assimp/anim.h>

namespace algine {
Animation::Animation(const aiAnimation *anim)
    : m_ticksPerSecond(anim->mTicksPerSecond),
      m_duration(anim->mDuration),
      m_name(anim->mName.data)
{
    m_channels.reserve(anim->mNumChannels); // allocate space

    for (uint i = 0; i < anim->mNumChannels; i++) {
        m_channels.emplace_back(anim->mChannels[i]);
    }
}
}