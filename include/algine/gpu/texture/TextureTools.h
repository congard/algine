#ifndef ALGINE_TEXTURETOOLS_H
#define ALGINE_TEXTURETOOLS_H

#include <algine/core/platform.h>
#include <algine/core/types.h>
#include <algine/gpu/texture/PixelData.h>

#include <string>

namespace algine::TextureTools {
AL_EXPORT void save(const std::string &path, const PixelData &pixelData, uint outComponents, bool flip = true);
}

#endif //ALGINE_TEXTURETOOLS_H
