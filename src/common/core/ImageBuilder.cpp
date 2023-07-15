#include <algine/core/ImageBuilder.h>
#include <algine/core/texture/Texture.h>

using namespace std;

namespace algine {
ImageBuilder::ImageBuilder()
    : m_format(),
      m_width(),
      m_height() {}

void ImageBuilder::setFormat(uint format) {
    m_format = format;
}

void ImageBuilder::setWidth(uint width) {
    m_width = width;
}

void ImageBuilder::setHeight(uint height) {
    m_height = height;
}

uint ImageBuilder::getFormat() const {
    return m_format;
}

uint ImageBuilder::getWidth() const {
    return m_width;
}

uint ImageBuilder::getHeight() const {
    return m_height;
}
}
