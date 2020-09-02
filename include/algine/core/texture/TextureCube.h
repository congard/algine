#ifndef ALGINE_TEXTURECUBE_H
#define ALGINE_TEXTURECUBE_H

#include <algine/core/texture/Texture.h>
#include <algine/core/texture/TextureCubePtr.h>
#include <algine/core/DataType.h>
#include <algine/templates.h>
#include <algine/types.h>

#include <vector>

namespace algine {
class TextureCube: public Texture {
public:
    enum class Face {
        Right = 0x8515,
        Left = 0x8516,
        Top = 0x8517,
        Bottom = 0x8518,
        Back = 0x8519,
        Front = 0x851A
    };

public:
    TextureCube();

    void fromFile(const std::string &path, Face face, DataType dataType = DataType::UnsignedByte, bool flipImage = false);
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

public:
    static TextureCubePtr getByName(const std::string &name);
    static TextureCube* byName(const std::string &name);

public:
    static std::vector<TextureCubePtr> publicTextures;
};
}

#endif //ALGINE_TEXTURECUBE_H
