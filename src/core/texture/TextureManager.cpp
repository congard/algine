#include <algine/core/texture/TextureManager.h>

#include <algine/core/texture/Texture.h>
#include <algine/core/JsonHelper.h>

#include "../TextureConfigTools.h"

using namespace std;
using namespace nlohmann;

namespace algine {
TextureManager::TextureManager()
    : m_type(),
      m_dataType(DataType::UnsignedByte),
      m_writeFileSection()
{
    // m_defaultParams initialized in derived class
    // width & height will be overwritten if texture loaded from file
    m_format = Texture::RGB;
}

void TextureManager::setType(Type type) {
    m_type = type;
}

void TextureManager::setDataType(DataType dataType) {
    m_dataType = dataType;
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

    // load params
    if (config.contains(Params)) {
        for (const auto & p : config[Params].items()) {
            m_params[stringToParamKey(p.key())] = stringToParamValue(p.value());
        }
    } else {
        m_params = m_defaultParams;
    }

    ImageManagerBase::import(jsonHelper);
}

JsonHelper TextureManager::dump() {
    using namespace Config;

    json config;

    // write type
    config[Config::Type] = vector<string> {Texture2D, TextureCube} [static_cast<uint>(m_type)];

    // write data type
    if (m_writeFileSection)
        config[File][Config::DataType] = dataTypeToString(m_dataType);

    // write params
    for (const auto & param : m_params)
        config[Params][paramKeyToString(param.first)] = paramValueToString(param.second);

    JsonHelper result(config);
    result.append(ImageManagerBase::dump());

    return result;
}
}
