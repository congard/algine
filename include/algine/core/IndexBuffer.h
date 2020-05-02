#ifndef ALGINE_INDEXBUFFER_H
#define ALGINE_INDEXBUFFER_H

#include <algine/core/Buffer.h>
#include <algine/templates.h>

namespace algine {
class IndexBuffer: public Buffer {
public:
    IndexBuffer();

    implementVariadicCreate(IndexBuffer)
    implementVariadicDestroy(IndexBuffer)
};
}

#endif //ALGINE_INDEXBUFFER_H
