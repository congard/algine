#include <algine/std/animation/Animation.h>
#include <algine/types.h>

#include <assimp/anim.h>

namespace algine {
Animation::Animation(const aiAnimation *anim) {
    ticksPerSecond = anim->mTicksPerSecond;
    duration = anim->mDuration;
    name = anim->mName.data;
    channels.reserve(anim->mNumChannels); // allocate space

    for (uint i = 0; i < anim->mNumChannels; i++)
        channels.emplace_back(anim->mChannels[i]);
}
}