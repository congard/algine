#include <algine/core/ImageManagerBase.h>

#include <algine/core/JsonHelper.h>

#include "ImageConfigTools.h"

using namespace std;
using namespace nlohmann;

namespace algine {
ImageManagerBase::ImageManagerBase()
    : m_format(),
      m_width(),
      m_height()
{
    // see initializer list above
}

void ImageManagerBase::setFormat(uint format) {
    m_format = format;
}

void ImageManagerBase::setWidth(uint width) {
    m_width = width;
}

void ImageManagerBase::setHeight(uint height) {
    m_height = height;
}

uint ImageManagerBase::getFormat() const {
    return m_format;
}

uint ImageManagerBase::getWidth() const {
    return m_width;
}

uint ImageManagerBase::getHeight() const {
    return m_height;
}

void ImageManagerBase::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load width & height
    m_width = jsonHelper.readValue<int>(Width);
    m_height = jsonHelper.readValue<int>(Height);

    // load format
    if (config.contains(Format))
        m_format = stringToFormat(config[Format]);

    ManagerBase::import(jsonHelper);
}

JsonHelper ImageManagerBase::dump() {
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
    result.append(ManagerBase::dump());

    return result;
}
}
