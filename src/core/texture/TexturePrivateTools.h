#ifndef ALGINE_TEXTUREPRIVATETOOLS_H
#define ALGINE_TEXTUREPRIVATETOOLS_H

#include <algine/types.h>
#include <algine/gl.h>

namespace algine::TexturePrivateTools {
inline uint getTexParam(const uint target, const uint name) {
    constexpr uint mipLevel = 0;

    int value;
    glGetTexLevelParameteriv(target, mipLevel, name, &value);

    return value;
}
}

#endif //ALGINE_TEXTUREPRIVATETOOLS_H
