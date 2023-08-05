#ifndef ALGINE_ARRAYBUFFER_H
#define ALGINE_ARRAYBUFFER_H

#include <algine/core/buffers/Buffer.h>

namespace algine {
class ArrayBuffer: public Buffer {
    AL_CONTEXT_OBJECT_IMPL(ArrayBuffer)

public:
    explicit ArrayBuffer(Object *parent = defaultParent());
};
}

#endif //ALGINE_ARRAYBUFFER_H
