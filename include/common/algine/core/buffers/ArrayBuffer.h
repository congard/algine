#ifndef ALGINE_ARRAYBUFFER_H
#define ALGINE_ARRAYBUFFER_H

#include <algine/core/buffers/Buffer.h>

namespace algine {
class ArrayBuffer: public Buffer {
    AL_CONTEXT_OBJECT_IMPL(ArrayBuffer)

public:
    ArrayBuffer();
};
}

#endif //ALGINE_ARRAYBUFFER_H
