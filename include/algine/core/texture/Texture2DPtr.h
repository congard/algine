#ifndef ALGINE_TEXTURE2DPTR_H
#define ALGINE_TEXTURE2DPTR_H

#include <memory>

namespace algine {
class Texture2D;

typedef std::shared_ptr<Texture2D> Texture2DPtr;
}

#endif //ALGINE_TEXTURE2DPTR_H
