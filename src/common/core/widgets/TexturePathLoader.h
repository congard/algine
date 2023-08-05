#ifndef ALGINE_TEXTUREPATHLOADER_H
#define ALGINE_TEXTUREPATHLOADER_H

#include <algine/core/texture/Texture2DBuilder.h>

#include <cstring>

namespace algine::TexturePathLoader {
inline Texture2D* load(std::string_view str_v, const std::shared_ptr<IOSystem> &io, Object *parent) {
    auto str = str_v.data();

    if (str_v.size() >= 4 && std::string_view(str + str_v.size() - 4) == ".lua") {
        Texture2DBuilder builder;
        builder.setIOSystem(io);
        builder.setParent(parent);
        builder.execute(str);
        return builder.get();
    } else {
        TextureFileInfo info;
        info.flip = true;
        info.path = str;
        info.ioSystem = io;

        auto texture = new Texture2D(parent);
        texture->bind();
        texture->fromFile(info);

        return texture;
    }
}
}

#endif //ALGINE_TEXTUREPATHLOADER_H
