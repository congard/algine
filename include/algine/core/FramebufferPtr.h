#ifndef ALGINE_FRAMEBUFFERPTR_H
#define ALGINE_FRAMEBUFFERPTR_H

#include <memory>

namespace algine {
class Framebuffer;

typedef std::shared_ptr<Framebuffer> FramebufferPtr;
}

#endif //ALGINE_FRAMEBUFFERPTR_H
