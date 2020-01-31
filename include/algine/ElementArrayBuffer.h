#ifndef ALGINE_ELEMENTARRAYBUFFER_H
#define ALGINE_ELEMENTARRAYBUFFER_H

#include <algine/Buffer.h>
#include <algine/templates.h>

namespace algine {
class ElementArrayBuffer: public Buffer {
public:
    ElementArrayBuffer();

    implementVariadicCreate(ElementArrayBuffer)
    implementVariadicDestroy(ElementArrayBuffer)
};
}

#endif //ALGINE_ELEMENTARRAYBUFFER_H
