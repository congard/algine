#ifndef ALGINE_RENDERBUFFERCREATOR_H
#define ALGINE_RENDERBUFFERCREATOR_H

#include <algine/core/ImageCreator.h>
#include <algine/core/RenderbufferPtr.h>
#include <algine/core/Renderbuffer.h>

namespace algine {
class RenderbufferCreator: public ImageCreator {
public:
    RenderbufferPtr get();
    RenderbufferPtr create();
};
}

#endif //ALGINE_RENDERBUFFERCREATOR_H
