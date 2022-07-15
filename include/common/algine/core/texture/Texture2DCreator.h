#ifndef ALGINE_TEXTURE2DCREATOR_H
#define ALGINE_TEXTURE2DCREATOR_H

#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/Texture2DPtr.h>

namespace algine {
class Texture2DCreator: public TextureCreator {
public:
    Texture2DCreator();

    void setPath(std::string_view path);
    const std::string& getPath() const;

    Texture2DPtr get();
    Texture2DPtr create();

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    std::string m_path;
};
}

#endif //ALGINE_TEXTURE2DCREATOR_H
