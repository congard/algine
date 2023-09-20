#ifndef ALGINE_TEXTUREBUILDER_H
#define ALGINE_TEXTUREBUILDER_H

#include <algine/core/ImageBuilder.h>
#include <algine/core/DataType.h>

#include <map>

namespace algine {
class AL_EXPORT TextureBuilder: public ImageBuilder {
public:
    enum class Type {
        Texture2D,
        TextureCube
    };

public:
    TextureBuilder();

    void setType(Type type);
    void setDataType(DataType dataType);

    void setParams(const std::map<uint, uint> &params);
    void setDefaultParams(const std::map<uint, uint> &defaultParams);

    Type getType() const;
    DataType getDataType() const;

    const std::map<uint, uint>& getParams() const;
    const std::map<uint, uint>& getDefaultParams() const;

protected:
    Type m_type;
    DataType m_dataType;
    std::map<uint, uint> m_params;

protected:
    std::map<uint, uint> m_defaultParams;
};
}

#endif //ALGINE_TEXTUREBUILDER_H
