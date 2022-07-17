#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/Texture.h>

#include <algine/core/lua/DataType.h>

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

void TextureCreator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "TextureCreator"))
        return;

    lua->registerUsertype<ImageCreator, lua::DataType>(tenv);

    auto usertype = env.new_usertype<TextureCreator>(
            "TextureCreator",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable, IOProvider, Creator, ImageCreator>());

    usertype["getType"] = &TextureCreator::getType;
    usertype["type"] = sol::readonly_property(&TextureCreator::getType);

    Lua::new_property(usertype, "dataType", &TextureCreator::getDataType, &TextureCreator::setDataType);
    Lua::new_property(usertype, "params", &TextureCreator::getParams,
        [](TextureCreator &self, std::map<uint, uint> params) { self.setParams(params); });
    Lua::new_property(usertype, "defaultParams", &TextureCreator::getDefaultParams,
        [](TextureCreator &self, std::map<uint, uint> params) { self.setDefaultParams(params); });
}
}
