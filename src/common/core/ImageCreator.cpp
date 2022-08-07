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
}
