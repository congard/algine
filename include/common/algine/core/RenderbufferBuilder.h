#ifndef ALGINE_RENDERBUFFERBUILDER_H
#define ALGINE_RENDERBUFFERBUILDER_H

#include <algine/core/ImageBuilder.h>
#include <algine/core/Renderbuffer.h>

namespace algine {
class RenderbufferBuilder: public ImageBuilder {
    AL_BUILDER(Renderbuffer)

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;
};
}

#endif //ALGINE_RENDERBUFFERBUILDER_H