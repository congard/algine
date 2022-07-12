#ifndef ALGINE_RENDERBUFFERCREATOR_H
#define ALGINE_RENDERBUFFERCREATOR_H

#include <algine/core/ImageCreator.h>
#include <algine/core/RenderbufferPtr.h>

namespace algine {
class RenderbufferCreator: public ImageCreator {
public:
    RenderbufferPtr get();
    RenderbufferPtr create();

    static void registerLuaUsertype(Lua *lua);

protected:
    void exec(const std::string &s, bool path, Lua *lua) override;
};
}

#endif //ALGINE_RENDERBUFFERCREATOR_H
