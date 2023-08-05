#ifndef ALGINE_TEXTURECUBE_H
#define ALGINE_TEXTURECUBE_H

#include <algine/core/texture/Texture.h>
#include <algine/core/DataType.h>
#include <algine/templates.h>
#include <algine/types.h>

#include <vector>

namespace algine {
class TextureCube: public Texture {
    AL_CONTEXT_OBJECT_IMPL(TextureCube)

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
    explicit TextureCube(Object *parent = defaultParent());

    void fromFile(Face face, const TextureFileInfo &fileInfo);
    void fromFile(Face face, const std::string &path);
    void update() override;

    static std::map<uint, uint> defaultParams();

    template<typename T, typename...Args>
    static void setParamsMultiple(const std::map<uint, T> &params, Args...args) {
        TextureCube* arr[] = {args...};

        for (usize i = 0; i < sizeof...(args); i++) {
            arr[i]->bind();
            arr[i]->setParams(params);
        }
    }

    uint getActualFormat() const override;
    uint getActualWidth() const override;
    uint getActualHeight() const override;
};
}

#endif //ALGINE_TEXTURECUBE_H
