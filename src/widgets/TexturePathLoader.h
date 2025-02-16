#ifndef ALGINE_TEXTUREPATHLOADER_H
#define ALGINE_TEXTUREPATHLOADER_H

#include <algine/gpu/texture/Texture2DBuilder.h>

#include <cstring>

namespace algine::TexturePathLoader {
inline Texture2D* load(std::string_view str_v, const std::shared_ptr<IOSystem> &io, Object *parent) {
    if (str_v.ends_with(".lua")) {
        Texture2DBuilder builder;
        builder.setIOSystem(io);
        builder.setParent(parent);
        builder.execute(std::string {str_v});
        return builder.get();
    } else {
        TextureFileInfo info;
        info.flip = true;
        info.path = str_v;
        info.ioSystem = io;

        auto texture = new Texture2D(parent);
        texture->bind();
        texture->fromFile(info);

        return texture;
    }
}
}

#endif //ALGINE_TEXTUREPATHLOADER_H
