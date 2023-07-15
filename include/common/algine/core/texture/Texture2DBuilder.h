#ifndef ALGINE_TEXTURE2DBUILDER_H
#define ALGINE_TEXTURE2DBUILDER_H

#include <algine/core/texture/TextureBuilder.h>
#include <algine/core/texture/Texture2DPtr.h>

namespace algine {
class Texture2DBuilder: public TextureBuilder {
public:
    Texture2DBuilder();

    void setPath(std::string_view path);
    const std::string& getPath() const;

    Texture2DPtr get();
    Texture2DPtr create();

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    std::string m_path;
};
}

#endif //ALGINE_TEXTURE2DBUILDER_H
