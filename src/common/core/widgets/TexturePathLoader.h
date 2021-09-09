#ifndef ALGINE_TEXTUREPATHLOADER_H
#define ALGINE_TEXTUREPATHLOADER_H

#include <algine/core/texture/Texture2DCreator.h>

#include <cstring>

namespace algine::TexturePathLoader {
struct Data {
    Texture2DPtr texture;
    bool unique;
};

inline Data load(std::string_view str_v, const std::shared_ptr<IOSystem> &io) {
    auto str = str_v.data();

    if (auto ext = strrchr(str, '.'); strcmp(ext, ".json") == 0) {
        Texture2DCreator creator;
        creator.setIOSystem(io);
        creator.importFromFile(str);

        return {
            creator.get(),
            creator.getAccess() == Creator::Access::Private
        };
    } else {
        TextureFileInfo info;
        info.flip = true;
        info.path = str;
        info.ioSystem = io;

        Texture2DPtr texture = PtrMaker::make();
        texture->bind();
        texture->fromFile(info);

        return {texture, true};
    }
}
}

#endif //ALGINE_TEXTUREPATHLOADER_H
