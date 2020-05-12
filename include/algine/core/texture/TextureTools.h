#ifndef ALGINE_TEXTURETOOLS_H
#define ALGINE_TEXTURETOOLS_H

#include <algine/core/texture/PixelData.h>
#include <algine/types.h>

#include <string>

namespace algine {
namespace TextureTools {
void saveImage(const std::string &path, PixelData &pixelData, uint outComponents, bool flip = true);
}
}

#endif //ALGINE_TEXTURETOOLS_H
