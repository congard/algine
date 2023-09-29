#ifndef ALGINE_TEXTURE2DBUILDER_H
#define ALGINE_TEXTURE2DBUILDER_H

#include <algine/core/texture/TextureBuilder.h>
#include <algine/core/texture/Texture2D.h>

namespace algine {
class AL_EXPORT Texture2DBuilder: public TextureBuilder {
    AL_BUILDER(Texture2D)

public:
    Texture2DBuilder();

    void setPath(std::string_view path);
    const std::string& getPath() const;

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;

private:
    std::string m_path;
};
}

#endif //ALGINE_TEXTURE2DBUILDER_H
