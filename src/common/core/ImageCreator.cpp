#include <algine/core/ImageCreator.h>
#include <algine/core/texture/Texture.h>

using namespace std;

namespace algine {
ImageCreator::ImageCreator()
    : m_format(),
      m_width(),
      m_height() {}

void ImageCreator::setFormat(uint format) {
    m_format = format;
}

void ImageCreator::setWidth(uint width) {
    m_width = width;
}

void ImageCreator::setHeight(uint height) {
    m_height = height;
}

uint ImageCreator::getFormat() const {
    return m_format;
}

uint ImageCreator::getWidth() const {
    return m_width;
}

uint ImageCreator::getHeight() const {
    return m_height;
}

void ImageCreator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "ImageCreator"))
        return;

    lua->registerUsertype<Creator, Texture>(tenv);

    auto usertype = env.new_usertype<ImageCreator>(
            "ImageCreator",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable, IOProvider, Creator>());

    Lua::new_property(usertype, "format", &ImageCreator::getFormat, &ImageCreator::setFormat);
    Lua::new_property(usertype, "width", &ImageCreator::getWidth, &ImageCreator::setWidth);
    Lua::new_property(usertype, "height", &ImageCreator::getHeight, &ImageCreator::setHeight);
}
}
