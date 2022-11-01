#include <algine/core/window/Icon.h>

// STB_IMAGE_IMPLEMENTATION defined in Texture.cpp
#include <stb_image.h>

#include "core/IOStreamUtils.h"

namespace algine {
Icon::Icon(): m_width(), m_height() {}

Icon::Icon(const std::string &path, const std::shared_ptr<IOSystem> &ioSystem) {
    auto bytes = IOStreamUtils::readAll<stbi_uc>(path, ioSystem);

    m_pixelData.reset(
        stbi_load_from_memory(bytes.array(), (int) bytes.size(), &m_width, &m_height, nullptr, 4), //rgba channels
        [](void *ptr) {
            stbi_image_free(ptr);
        }
    );
}

void Icon::setWidth(int width) {
    m_width = width;
}

void Icon::setHeight(int height) {
    m_height = height;
}

void Icon::setPixels(const PixelData &pixelData) {
    m_pixelData = pixelData;
}

int Icon::getWidth() const {
    return m_width;
}

int Icon::getHeight() const {
    return m_height;
}

const Icon::PixelData& Icon::getPixelData() const {
    return m_pixelData;
}
}
