#ifndef ALGINE_TEXTURECREATEINFO_H
#define ALGINE_TEXTURECREATEINFO_H

#include <algine/types.h>
#include <map>

namespace algine {
struct TextureCreateInfo {
    uint lod = 0;
    uint format = 0;
    uint width = 0, height = 0;
    std::map<uint, uint> params;
};
}

#endif //ALGINE_TEXTURECREATEINFO_H
