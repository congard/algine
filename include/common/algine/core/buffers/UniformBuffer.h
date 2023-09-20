#ifndef ALGINE_UNIFORMBUFFER_H
#define ALGINE_UNIFORMBUFFER_H

#include <algine/core/buffers/Buffer.h>

namespace algine {
class AL_EXPORT UniformBuffer: public Buffer {
    AL_CONTEXT_OBJECT_IMPL(UniformBuffer)

public:
    explicit UniformBuffer(Object *parent = defaultParent());

    static uint getOffsetAlignment();
};
}

#endif //ALGINE_UNIFORMBUFFER_H
