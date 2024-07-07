#ifndef ALGINE_PIXELDATA_H
#define ALGINE_PIXELDATA_H

#include <tulz/container/Array.h>
#include <algine/types.h>

namespace algine {
struct PixelData {
    uint width, height;
    uint componentsCount;
    tulz::Array<float> pixels;
};
}

#endif //ALGINE_PIXELDATA_H
