#include <algine/core/texture/TextureBuilder.h>
#include <algine/core/texture/Texture.h>

using namespace std;

namespace algine {
TextureBuilder::TextureBuilder()
    : m_type(),
      m_dataType(DataType::UnsignedByte)
{
    // m_defaultParams initialized in derived class
    // width & height will be overwritten if texture loaded from file
    m_format = Texture::RGB;
}

void TextureBuilder::setType(Type type) {
    m_type = type;
}

void TextureBuilder::setDataType(DataType dataType) {
    m_dataType = dataType;
}

void TextureBuilder::setParams(const map<uint, uint> &params) {
    m_params = params;
}

void TextureBuilder::setDefaultParams(const map<uint, uint> &defaultParams) {
    m_defaultParams = defaultParams;
}

TextureBuilder::Type TextureBuilder::getType() const {
    return m_type;
}

DataType TextureBuilder::getDataType() const {
    return m_dataType;
}

const map<uint, uint>& TextureBuilder::getParams() const {
    return m_params;
}

const map<uint, uint>& TextureBuilder::getDefaultParams() const {
    return m_defaultParams;
}
}
