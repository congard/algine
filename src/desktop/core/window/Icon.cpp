#include <algine/core/window/Icon.h>

// STB_IMAGE_IMPLEMENTATION defined in Texture.cpp
#include <stb/stb_image.h>

namespace algine {
Icon::Icon(): m_width(), m_height() {}

Icon::Icon(const std::string &path) {
    m_pixelData.reset(
        stbi_load(path.c_str(), &m_width, &m_height, nullptr, 4), //rgba channels
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
