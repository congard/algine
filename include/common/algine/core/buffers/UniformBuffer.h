#ifndef ALGINE_UNIFORMBUFFER_H
#define ALGINE_UNIFORMBUFFER_H

#include <algine/core/buffers/Buffer.h>
#include <algine/templates.h>

namespace algine {
class UniformBuffer: public Buffer {
public:
    UniformBuffer();

    static uint getOffsetAlignment();
};
}

#endif //ALGINE_UNIFORMBUFFER_H
