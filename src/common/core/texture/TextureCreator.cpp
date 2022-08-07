#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/Texture.h>

using namespace std;

namespace algine {
TextureCreator::TextureCreator()
    : m_type(),
      m_dataType(DataType::UnsignedByte)
{
    // m_defaultParams initialized in derived class
    // width & height will be overwritten if texture loaded from file
    m_format = Texture::RGB;
}

void TextureCreator::setType(Type type) {
    m_type = type;
}

void TextureCreator::setDataType(DataType dataType) {
    m_dataType = dataType;
}

void TextureCreator::setParams(const map<uint, uint> &params) {
    m_params = params;
}

void TextureCreator::setDefaultParams(const map<uint, uint> &defaultParams) {
    m_defaultParams = defaultParams;
}

TextureCreator::Type TextureCreator::getType() const {
    return m_type;
}

DataType TextureCreator::getDataType() const {
    return m_dataType;
}

const map<uint, uint>& TextureCreator::getParams() const {
    return m_params;
}

const map<uint, uint>& TextureCreator::getDefaultParams() const {
    return m_defaultParams;
}
}
