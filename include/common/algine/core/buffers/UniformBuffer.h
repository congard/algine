#ifndef ALGINE_UNIFORMBUFFER_H
#define ALGINE_UNIFORMBUFFER_H

#include <algine/core/buffers/Buffer.h>

namespace algine {
class UniformBuffer: public Buffer {
    AL_CONTEXT_OBJECT_IMPL(UniformBuffer)

public:
    UniformBuffer();

    static uint getOffsetAlignment();
};
}

#endif //ALGINE_UNIFORMBUFFER_H
