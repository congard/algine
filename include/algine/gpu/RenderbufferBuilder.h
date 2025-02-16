#ifndef ALGINE_RENDERBUFFERBUILDER_H
#define ALGINE_RENDERBUFFERBUILDER_H

#include <algine/gpu/ImageBuilder.h>
#include <algine/gpu/Renderbuffer.h>

namespace algine {
class AL_EXPORT RenderbufferBuilder: public ImageBuilder {
    AL_BUILDER(Renderbuffer)

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;
};
}

#endif //ALGINE_RENDERBUFFERBUILDER_H
