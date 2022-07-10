#ifndef ALGINE_TEXTURE2D_H
#define ALGINE_TEXTURE2D_H

#include <algine/core/texture/Texture.h>
#include <algine/core/texture/Texture2DPtr.h>
#include <algine/core/DataType.h>
#include <algine/templates.h>
#include <algine/types.h>

#include <vector>

namespace algine {
class Texture2D: public Texture {
public:
    Texture2D();

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

    static std::map<uint, uint> defaultParams();

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

    static void registerLuaUsertype(Lua *lua);

public:
    static Texture2DPtr getByName(const std::string &name);
    static Texture2D* byName(const std::string &name);

public:
    static std::vector<Texture2DPtr> publicObjects;
};
}

#endif //ALGINE_TEXTURE2D_H
