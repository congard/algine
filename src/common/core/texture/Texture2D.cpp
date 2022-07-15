#include <algine/core/texture/Texture2D.h>
#include <algine/core/Engine.h>

#include <tulz/Array.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTexture2D()
#define SOP_OBJECT_TYPE SOPConstants::Texture2DObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::Texture2DStr
#include "internal/SOP.h"
#include "internal/SOPConstants.h"

#include "internal/PublicObjectTools.h"

#include "TexturePrivateTools.h"

using namespace std;
using namespace algine::internal;
using namespace tulz;

namespace algine {
vector<Texture2DPtr> Texture2D::publicObjects;

Texture2D::Texture2D()
    : Texture(GL_TEXTURE_2D) {}

void Texture2D::fromFile(const TextureFileInfo &fileInfo) {
    checkBinding()
    texFromFile(GL_TEXTURE_2D, fileInfo);
}

void Texture2D::fromFile(const std::string &path) {
    fromFile(TextureFileInfo {path});
}

void Texture2D::update() {
    checkBinding()

    auto [dataFormat, dataType] = TexturePrivateTools::getDataInfo(m_format);

    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, static_cast<GLenum>(dataType), nullptr);
}

void Texture2D::update(uint dataFormat, DataType dataType, const void *data) {
    checkBinding()
    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, static_cast<GLenum>(dataType), data);
}

map<uint, uint> Texture2D::defaultParams() {
    return {
        {MinFilter, Linear},
        {MagFilter, Linear},
        {WrapU, ClampToEdge},
        {WrapV, ClampToEdge}
    };
}

uint Texture2D::getActualFormat() const {
    checkBinding()
    return TexturePrivateTools::getTexParam(m_target, GL_TEXTURE_INTERNAL_FORMAT);
}

uint Texture2D::getActualWidth() const {
    checkBinding()
    return TexturePrivateTools::getTexParam(m_target, GL_TEXTURE_WIDTH);
}

uint Texture2D::getActualHeight() const {
    checkBinding()
    return TexturePrivateTools::getTexParam(m_target, GL_TEXTURE_HEIGHT);
}

Texture2DPtr Texture2D::getByName(const string &name) {
    return PublicObjectTools::getByName<Texture2DPtr>(name);
}

Texture2D* Texture2D::byName(const string &name) {
    return PublicObjectTools::byName<Texture2D>(name);
}

void Texture2D::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "Texture2D"))
        return;

    lua->registerUsertype<Texture>(tenv);

    auto factories = sol::factories([]() { return PtrMaker::make<Texture2D>(); });
    auto usertype = env.new_usertype<Texture2D>(
            "Texture2D",
            sol::meta_function::construct, factories,
            sol::call_constructor, factories,
            sol::base_classes, sol::bases<Scriptable, Object, Texture>());

    usertype["fromFile"] = sol::overload(
        static_cast<void (Texture2D::*)(const TextureFileInfo&)>(&Texture2D::fromFile),
        static_cast<void (Texture2D::*)(const std::string&)>(&Texture2D::fromFile));
    usertype["update"] = sol::overload(
        static_cast<void (Texture2D::*)()>(&Texture2D::update),
        [](Texture2D &self, uint dataFormat, DataType dataType, const sol::object &data) {
            void *v = nullptr;

            if (data.is<Array<uint>>())
                v = data.as<Array<uint>>().array();
            else if (data.is<Array<int>>())
                v = data.as<Array<int>>().array();
            else if (data.is<Array<float>>())
                v = data.as<Array<float>>().array();
            else if (data.is<Array<double>>())
                v = data.as<Array<double>>().array();

            self.update(dataFormat, dataType, v);
        });

    // static
    usertype["defaultParams"] = &Texture2D::defaultParams;
    usertype["getByName"] = &Texture2D::getByName;
    usertype["byName"] = &Texture2D::byName;
}
}
