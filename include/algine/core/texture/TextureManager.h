#ifndef ALGINE_TEXTUREMANAGER_H
#define ALGINE_TEXTUREMANAGER_H

#include <algine/core/ManagerBase.h>
#include <algine/core/DataType.h>

#include <map>

namespace algine {
class TextureManager: public ManagerBase {
public:
    enum class Type {
        Texture2D,
        TextureCube
    };

public:
    TextureManager();

    void setType(Type type);
    void setDataType(DataType dataType);
    void setFormat(uint format);
    void setWidth(uint width);
    void setHeight(uint height);

    void setParams(const std::map<uint, uint> &params);
    void setDefaultParams(const std::map<uint, uint> &defaultParams);

    Type getType() const;
    DataType getDataType() const;
    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;

    const std::map<uint, uint>& getParams() const;
    const std::map<uint, uint>& getDefaultParams() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    Type m_type;
    DataType m_dataType;
    uint m_format;
    uint m_width, m_height;
    std::map<uint, uint> m_params;

protected:
    std::map<uint, uint> m_defaultParams;

protected:
    bool m_writeFileSection;
};
}

#endif //ALGINE_TEXTUREMANAGER_H
