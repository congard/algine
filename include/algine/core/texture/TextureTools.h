#ifndef ALGINE_TEXTURETOOLS_H
#define ALGINE_TEXTURETOOLS_H

#include <algine/core/texture/PixelData.h>
#include <algine/types.h>

#include <string>

namespace algine::TextureTools {
void save(const std::string &path, const PixelData &pixelData, uint outComponents, bool flip = true);
}

#endif //ALGINE_TEXTURETOOLS_H
