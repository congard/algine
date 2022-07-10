#include <algine/core/ImageCreator.h>

#include <algine/core/JsonHelper.h>

#include "ImageConfigTools.h"

using namespace std;
using namespace nlohmann;

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

void ImageCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load width & height
    m_width = jsonHelper.readValue<int>(Width);
    m_height = jsonHelper.readValue<int>(Height);

    // load format
    if (config.contains(Format))
        m_format = stringToFormat(config[Format]);

    Creator::import(jsonHelper);
}

JsonHelper ImageCreator::dump() {
    using namespace Config;

    json config;

    // write width & height if not zero
    if (m_width != 0)
        config[Width] = m_width;

    if (m_height != 0)
        config[Height] = m_height;

    // write format
    config[Format] = formatToString(m_format);

    JsonHelper result(config);
    result.append(Creator::dump());

    return result;
}

void ImageCreator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "ImageCreator"))
        return;

    lua->registerUsertype<Creator, Texture>();

    auto usertype = lua->state()->new_usertype<ImageCreator>(
            "ImageCreator",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator>());

    Lua::new_property(usertype, "format", "getFormat", "setFormat", &ImageCreator::getFormat, &ImageCreator::setFormat);
    Lua::new_property(usertype, "width", "getWidth", "setWidth", &ImageCreator::getWidth, &ImageCreator::setWidth);
    Lua::new_property(usertype, "height", "getHeight", "setHeight", &ImageCreator::getHeight, &ImageCreator::setHeight);
}
}
