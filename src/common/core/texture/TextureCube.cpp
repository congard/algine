#include <algine/core/texture/TextureCube.h>
#include <algine/core/Engine.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTextureCube()
#define SOP_OBJECT_TYPE SOPConstants::TextureCubeObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::TextureCubeStr
#include "internal/SOP.h"
#include "internal/SOPConstants.h"

#include "internal/PublicObjectTools.h"

#include "TexturePrivateTools.h"

using namespace std;
using namespace algine::internal;

namespace algine {
vector<TextureCubePtr> TextureCube::publicObjects;

TextureCube::TextureCube()
    : Texture(GL_TEXTURE_CUBE_MAP) {}

void TextureCube::fromFile(Face face, const TextureFileInfo &fileInfo) {
    checkBinding()
    texFromFile(static_cast<uint>(face), fileInfo);
}

void TextureCube::fromFile(Face face, const std::string &path) {
    fromFile(face, TextureFileInfo {path});
}

void TextureCube::update() {
    checkBinding()

    auto [dataFormat, dataType] = TexturePrivateTools::getDataInfo(m_format);

    for (uint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_lod, m_format, m_width, m_height, 0, dataFormat,
                     static_cast<GLenum>(dataType), nullptr);
    }
}

map<uint, uint> TextureCube::defaultParams() {
    return {
        {MinFilter, Linear},
        {MagFilter, Linear},
        {WrapU, ClampToEdge},
        {WrapV, ClampToEdge},
        {WrapW, ClampToEdge}
    };
}

// TextureCube faces have the same internal format, size, mipmap level
// https://www.khronos.org/opengl/wiki/Cubemap_Texture
#define target static_cast<uint>(Face::Right)

uint TextureCube::getActualFormat() const {
    return TexturePrivateTools::getTexParam(target, GL_TEXTURE_INTERNAL_FORMAT);
}

uint TextureCube::getActualWidth() const {
    return TexturePrivateTools::getTexParam(target, GL_TEXTURE_WIDTH);
}

uint TextureCube::getActualHeight() const {
    return TexturePrivateTools::getTexParam(target, GL_TEXTURE_HEIGHT);
}

#undef target

TextureCubePtr TextureCube::getByName(const string &name) {
    return PublicObjectTools::getByName<TextureCubePtr>(name);
}

TextureCube* TextureCube::byName(const string &name) {
    return PublicObjectTools::byName<TextureCube>(name);
}

void TextureCube::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "TextureCube"))
        return;

    lua->registerUsertype<Texture>(tenv);

    auto factories = sol::factories([]() { return PtrMaker::make<TextureCube>(); });
    auto usertype = env.new_usertype<TextureCube>(
            "TextureCube",
            sol::meta_function::construct, factories,
            sol::call_constructor, factories,
            sol::base_classes, sol::bases<Scriptable, Object, Texture>());

    usertype["fromFile"] = sol::overload(
            static_cast<void (TextureCube::*)(Face, const TextureFileInfo&)>(&TextureCube::fromFile),
            static_cast<void (TextureCube::*)(Face, const std::string&)>(&TextureCube::fromFile));
    usertype["update"] = &TextureCube::update;

    // static
    usertype["defaultParams"] = &TextureCube::defaultParams;
    usertype["getByName"] = &TextureCube::getByName;
    usertype["byName"] = &TextureCube::byName;

    // enums
    env["TextureCube"].get<sol::table>().new_enum("Face",
        "Right", Face::Right,
        "Left", Face::Left,
        "Top", Face::Top,
        "Bottom", Face::Bottom,
        "Back", Face::Back,
        "Front", Face::Front);
}
}
