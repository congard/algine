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

    if (str_v.size() >= 4 && std::string_view(str + str_v.size() - 4) == ".lua") {
        Texture2DCreator creator;
        creator.setIOSystem(io);
        creator.execute(str);

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
