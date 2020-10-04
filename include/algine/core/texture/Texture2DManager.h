#ifndef ALGINE_TEXTURE2DMANAGER_H
#define ALGINE_TEXTURE2DMANAGER_H

#include <algine/core/texture/TextureManager.h>
#include <algine/core/texture/Texture2DPtr.h>

namespace algine {
class Texture2DManager: public TextureManager {
public:
    Texture2DManager();

    void setPath(const std::string &path);
    std::string getPath() const;

    Texture2DPtr get();
    Texture2DPtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    std::string m_path;
};
}

#endif //ALGINE_TEXTURE2DMANAGER_H
