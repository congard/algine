#ifndef ALGINE_TEXTUREMANAGER_H
#define ALGINE_TEXTUREMANAGER_H

#include <algine/core/ImageManagerBase.h>
#include <algine/core/DataType.h>

#include <map>

namespace algine {
class TextureManager: public ImageManagerBase {
public:
    enum class Type {
        Texture2D,
        TextureCube
    };

public:
    TextureManager();

    void setType(Type type);
    void setDataType(DataType dataType);

    void setParams(const std::map<uint, uint> &params);
    void setDefaultParams(const std::map<uint, uint> &defaultParams);

    Type getType() const;
    DataType getDataType() const;

    const std::map<uint, uint>& getParams() const;
    const std::map<uint, uint>& getDefaultParams() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    Type m_type;
    DataType m_dataType;
    std::map<uint, uint> m_params;

protected:
    std::map<uint, uint> m_defaultParams;

protected:
    bool m_writeFileSection;
};
}

#endif //ALGINE_TEXTUREMANAGER_H
