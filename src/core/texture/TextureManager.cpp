#include <algine/core/texture/TextureManager.h>

#include <algine/core/texture/Texture.h>
#include <algine/core/JsonHelper.h>

#include "../ConfigStrings.h"

using namespace std;
using namespace nlohmann;

namespace algine {
TextureManager::TextureManager()
    : m_type(),
      m_dataType(DataType::UnsignedByte),
      m_format(Texture::RGB),
      m_width(), // width & height will be overwritten
      m_height(), // if texture loaded from file
      m_writeFileSection()
{
    // m_defaultParams initialized in derived class
}

void TextureManager::setType(Type type) {
    m_type = type;
}

void TextureManager::setDataType(DataType dataType) {
    m_dataType = dataType;
}

void TextureManager::setFormat(uint format) {
    m_format = format;
}

void TextureManager::setWidth(uint width) {
    m_width = width;
}

void TextureManager::setHeight(uint height) {
    m_height = height;
}

void TextureManager::setParams(const map<uint, uint> &params) {
    m_params = params;
}

void TextureManager::setDefaultParams(const map<uint, uint> &defaultParams) {
    m_defaultParams = defaultParams;
}

TextureManager::Type TextureManager::getType() const {
    return m_type;
}

DataType TextureManager::getDataType() const {
    return m_dataType;
}

uint TextureManager::getFormat() const {
    return m_format;
}

uint TextureManager::getWidth() const {
    return m_width;
}

uint TextureManager::getHeight() const {
    return m_height;
}

const map<uint, uint>& TextureManager::getParams() const {
    return m_params;
}

const map<uint, uint>& TextureManager::getDefaultParams() const {
    return m_defaultParams;
}

void TextureManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load type
    m_type = map<string, Type> {
        {Texture2D, Type::Texture2D},
        {TextureCube, Type::TextureCube}
    } [config[Config::Type]];

    // load data type
    if (config.contains(File) && config[File].contains(Config::DataType))
        m_dataType = stringToDataType(config[File][Config::DataType]);

    // load width & height
    m_width = jsonHelper.readValue<int>(Width);
    m_height = jsonHelper.readValue<int>(Height);

    // load format
    if (config.contains(Format))
        m_format = stringToFormat(config[Format]);

    // load params
    if (config.contains(Params)) {
        for (const auto & p : config[Params].items()) {
            m_params[stringToParamKey(p.key())] = stringToParamValue(p.value());
        }
    } else {
        m_params = m_defaultParams;
    }

    ManagerBase::import(jsonHelper);
}

JsonHelper TextureManager::dump() {
    using namespace Config;

    json config;

    // write type
    config[Config::Type] = vector<string> {Texture2D, TextureCube} [static_cast<uint>(m_type)];

    // write data type
    if (m_writeFileSection)
        config[File][Config::DataType] = dataTypeToString(m_dataType);

    // write width & height if not zero
    if (m_width != 0)
        config[Width] = m_width;

    if (m_height != 0)
        config[Height] = m_height;

    // write format
    config[Format] = formatToString(m_format);

    // write params
    for (const auto & param : m_params)
        config[Params][paramKeyToString(param.first)] = paramValueToString(param.second);

    JsonHelper result(config);
    result.append(ManagerBase::dump());

    return result;
}
}
