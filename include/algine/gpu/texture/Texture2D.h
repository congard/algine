#ifndef ALGINE_TEXTURE2D_H
#define ALGINE_TEXTURE2D_H

#include <algine/core/types.h>
#include <algine/gpu/DataType.h>
#include <algine/gpu/texture/Texture.h>

#include <vector>

namespace algine {
class AL_EXPORT Texture2D: public Texture {
    AL_CONTEXT_OBJECT_IMPL(Texture2D)

public:
    explicit Texture2D(Object *parent = defaultParent());

    void fromFile(const TextureFileInfo &fileInfo);
    void fromFile(const std::string &path);
    void update() override;

    /**
     * updates width / height, lod, format
     * @param dataFormat
     * @param dataType
     * @param data
     */
    void update(uint dataFormat, DataType dataType, const void *data);

    /// #solgen #ignore
    static std::map<uint, uint> defaultParams();

    // TODO: remove?
    template<typename T, typename...Args>
    static void setParamsMultiple(const std::map<uint, T> &params, Args...args) {
        Texture2D* arr[] = {args...};

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

#endif //ALGINE_TEXTURE2D_H
