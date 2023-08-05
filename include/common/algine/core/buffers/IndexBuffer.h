#ifndef ALGINE_INDEXBUFFER_H
#define ALGINE_INDEXBUFFER_H

#include <algine/core/buffers/Buffer.h>

namespace algine {
class IndexBuffer: public Buffer {
    AL_CONTEXT_OBJECT_IMPL(IndexBuffer)

public:
    explicit IndexBuffer(Object *parent = defaultParent());
};
}

#endif //ALGINE_INDEXBUFFER_H
