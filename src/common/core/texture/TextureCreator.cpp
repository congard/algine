#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/Texture.h>
#include <algine/core/JsonHelper.h>

#include <algine/core/lua/DataType.h>

#include "../TextureConfigTools.h"

using namespace std;
using namespace nlohmann;

namespace algine {
TextureCreator::TextureCreator()
    : m_type(),
      m_dataType(DataType::UnsignedByte),
      m_writeFileSection()
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

void TextureCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load type
    m_type = map<string, Type> {
        {Config::Texture2D, Type::Texture2D},
        {Config::TextureCube, Type::TextureCube}
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

    ImageCreator::import(jsonHelper);
}

JsonHelper TextureCreator::dump() {
    using namespace Config;

    json config;

    // write type
    config[Config::Type] = vector<string> {Config::Texture2D, Config::TextureCube} [static_cast<uint>(m_type)];

    // write data type
    if (m_writeFileSection)
        config[File][Config::DataType] = dataTypeToString(m_dataType);

    // write params
    for (const auto & param : m_params)
        config[Params][paramKeyToString(param.first)] = paramValueToString(param.second);

    JsonHelper result(config);
    result.append(ImageCreator::dump());

    return result;
}

void TextureCreator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "TextureCreator"))
        return;

    lua->registerUsertype<ImageCreator, lua::DataType>();

    auto usertype = lua->state()->new_usertype<TextureCreator>(
            "TextureCreator",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator, ImageCreator>());

    usertype["getType"] = &TextureCreator::getType;
    usertype["type"] = sol::readonly_property(&TextureCreator::getType);

    Lua::new_property(usertype, "dataType", "getDataType", "setDataType", &TextureCreator::getDataType, &TextureCreator::setDataType);
    Lua::new_property(usertype, "params", "getParams", "setParams", &TextureCreator::getParams,
                      [](TextureCreator &self, std::map<uint, uint> params) { self.setParams(params); });
    Lua::new_property(usertype, "defaultParams", "getDefaultParams", "setDefaultParams", &TextureCreator::getDefaultParams,
                      [](TextureCreator &self, std::map<uint, uint> params) { self.setDefaultParams(params); });
}
}
