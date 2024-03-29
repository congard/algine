#ifndef ALGINE_TEXTURECUBECREATOR_H
#define ALGINE_TEXTURECUBECREATOR_H

#include <algine/core/texture/TextureBuilder.h>
#include <algine/core/texture/TextureCube.h>

namespace algine {
class AL_EXPORT TextureCubeBuilder: public TextureBuilder {
    AL_BUILDER(TextureCube)

public:
    TextureCubeBuilder();

    void setPath(std::string_view path, TextureCube::Face face);
    const std::string& getPath(TextureCube::Face face) const;

    void setPaths(const std::map<TextureCube::Face, std::string> &paths);
    const std::map<TextureCube::Face, std::string>& getPaths() const;

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;

private:
    std::map<TextureCube::Face, std::string> m_paths;
};
}

#endif //ALGINE_TEXTURECUBECREATOR_H
