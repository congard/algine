#ifndef ALGINE_RENDERBUFFERMANAGER_H
#define ALGINE_RENDERBUFFERMANAGER_H

#include <algine/core/ImageManagerBase.h>
#include <algine/core/RenderbufferPtr.h>
#include <algine/core/Renderbuffer.h>

namespace algine {
class RenderbufferManager: public ImageManagerBase {
public:
    RenderbufferPtr getRenderbuffer();
    RenderbufferPtr createRenderbuffer();
};
}

#endif //ALGINE_RENDERBUFFERMANAGER_H
