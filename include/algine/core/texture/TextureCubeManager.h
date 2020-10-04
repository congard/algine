#ifndef ALGINE_TEXTURECUBEMANAGER_H
#define ALGINE_TEXTURECUBEMANAGER_H

#include <algine/core/texture/TextureManager.h>
#include <algine/core/texture/TextureCubePtr.h>
#include <algine/core/texture/TextureCube.h>

namespace algine {
class TextureCubeManager: public TextureManager {
public:
    TextureCubeManager();

    void setPath(const std::string &path, TextureCube::Face face);
    std::string getPath(TextureCube::Face face) const;

    void setPaths(const std::map<TextureCube::Face, std::string> &paths);
    const std::map<TextureCube::Face, std::string>& getPaths() const;

    TextureCubePtr get();
    TextureCubePtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    std::map<TextureCube::Face, std::string> m_paths;
};
}

#endif //ALGINE_TEXTURECUBEMANAGER_H
