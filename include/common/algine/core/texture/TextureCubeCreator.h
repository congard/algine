#ifndef ALGINE_TEXTURECUBECREATOR_H
#define ALGINE_TEXTURECUBECREATOR_H

#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/TextureCubePtr.h>
#include <algine/core/texture/TextureCube.h>

namespace algine {
class TextureCubeCreator: public TextureCreator {
public:
    TextureCubeCreator();

    void setPath(std::string_view path, TextureCube::Face face);
    const std::string& getPath(TextureCube::Face face) const;

    void setPaths(const std::map<TextureCube::Face, std::string> &paths);
    const std::map<TextureCube::Face, std::string>& getPaths() const;

    TextureCubePtr get();
    TextureCubePtr create();

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    std::map<TextureCube::Face, std::string> m_paths;
};
}

#endif //ALGINE_TEXTURECUBECREATOR_H
