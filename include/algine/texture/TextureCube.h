#ifndef ALGINE_TEXTURECUBE_H
#define ALGINE_TEXTURECUBE_H

#include <algine/templates.h>
#include <algine/types.h>
#include <algine/texture/Texture.h>

namespace algine {
class TextureCube: public Texture {
public:
    enum Faces {
        Right = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        Left = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        Top = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        Bottom = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        Back = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        Front = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    TextureCube();

    void fromFile(const std::string &path, uint face, uint dataType = GL_UNSIGNED_BYTE, bool flipImage = false);
    void update() override;

    static std::map<uint, uint> defaultParams();

    template<typename T, typename...Args>
    static void setParamsMultiple(const std::map<uint, T> &params, Args&...args) {
        TextureCube** arr[] = {&args...};
        for (usize i = 0; i < sizeof...(args); i++) {
            (*arr[i])->bind();
            (*arr[i])->setParams(params);
        }
    }

    implementVariadicCreate(TextureCube)
    implementVariadicDestroy(TextureCube)
};
}

#endif //ALGINE_TEXTURECUBE_H
