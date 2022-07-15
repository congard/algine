#ifndef ALGINE_TEXTURECREATOR_H
#define ALGINE_TEXTURECREATOR_H

#include <algine/core/ImageCreator.h>
#include <algine/core/DataType.h>

#include <map>

namespace algine {
class TextureCreator: public ImageCreator {
public:
    enum class Type {
        Texture2D,
        TextureCube
    };

public:
    TextureCreator();

    void setType(Type type);
    void setDataType(DataType dataType);

    void setParams(const std::map<uint, uint> &params);
    void setDefaultParams(const std::map<uint, uint> &defaultParams);

    Type getType() const;
    DataType getDataType() const;

    const std::map<uint, uint>& getParams() const;
    const std::map<uint, uint>& getDefaultParams() const;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    Type m_type;
    DataType m_dataType;
    std::map<uint, uint> m_params;

protected:
    std::map<uint, uint> m_defaultParams;
};
}

#endif //ALGINE_TEXTURECREATOR_H
