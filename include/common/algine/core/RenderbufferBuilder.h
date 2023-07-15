#ifndef ALGINE_RENDERBUFFERBUILDER_H
#define ALGINE_RENDERBUFFERBUILDER_H

#include <algine/core/ImageBuilder.h>
#include <algine/core/RenderbufferPtr.h>

namespace algine {
class RenderbufferBuilder: public ImageBuilder {
public:
    RenderbufferPtr get();
    RenderbufferPtr create();

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;
};
}

#endif //ALGINE_RENDERBUFFERBUILDER_H
