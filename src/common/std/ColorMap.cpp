#include <algine/std/ColorMap.h>
#include <algine/core/PtrMaker.h>

#include <glm/gtx/string_cast.hpp>

#include <tulz/Array.h>

#include <stdexcept>

namespace algine {
ColorMap::ColorMap() = default;

ColorMap::ColorMap(uint width, uint height, uint format) {
    create(width, height, format);
}

void ColorMap::create(uint width, uint height, uint format) {
    m_texture = PtrMaker::make();
    m_texture->bind();
    m_texture->setDimensions(width, height);
    m_texture->setFormat(format);
    m_texture->setParams(std::map<uint, uint> {
        {Texture::MinFilter, Texture::Nearest},
        {Texture::MagFilter, Texture::Nearest},
        {Texture::WrapU, Texture::ClampToEdge},
        {Texture::WrapV, Texture::ClampToEdge}
    });
}

inline auto componentsCount(uint format) {
    switch (format) {
        case Texture::Red:
        case Texture::RedInteger:
            return 1;
        case Texture::RG:
        case Texture::RGInteger:
            return 2;
        case Texture::RGB:
        case Texture::RGBInteger:
            return 3;
        case Texture::RGBA:
        case Texture::RGBAInteger:
            return 4;
        default: throw std::invalid_argument("Invalid format");
    }
}

template<typename T>
inline void fillData(T *data, const ColorMap::Colors &colors, uint components, uint width) {
    switch (components) {
        case 1:
            for (const auto &p : colors) {
                auto &color = p.first;
                auto &uv = p.second;
                data[uv.y * width + uv.x] = color.red<T>();
            }
            break;
        case 2:
            for (const auto &p : colors) {
                auto &color = p.first;
                auto &uv = p.second;
                auto index = (uv.y * width + uv.x) * 2;
                data[index + 0] = color.red<T>();
                data[index + 1] = color.green<T>();
            }
            break;
        case 3:
            for (const auto &p : colors) {
                auto &color = p.first;
                auto &uv = p.second;
                auto index = (uv.y * width + uv.x) * 3;
                data[index + 0] = color.red<T>();
                data[index + 1] = color.green<T>();
                data[index + 2] = color.blue<T>();
            }

            break;
        case 4:
            for (const auto &p : colors) {
                auto &color = p.first;
                auto &uv = p.second;
                auto index = (uv.y * width + uv.x) * 4;
                data[index + 0] = color.red<T>();
                data[index + 1] = color.green<T>();
                data[index + 2] = color.blue<T>();
                data[index + 3] = color.alpha<T>();
            }
            break;
        default: break;
    }
}

void ColorMap::update() {
    auto width = getWidth();
    auto height = getHeight();

    auto [baseFormat, dataType] = Texture::getFormatInfo(m_texture->getFormat());
    auto components = componentsCount(baseFormat);

    void *data = calloc(components * width * height, 4);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // TODO: to Engine

    m_texture->bind();

    if (dataType == DataType::Float || dataType == DataType::HalfFloat) {
        fillData((float*) data, m_colors, components, width);
        m_texture->update(baseFormat, DataType::Float, data);
    } else {
        fillData((int*) data, m_colors, components, width);
        m_texture->update(baseFormat, DataType::Int, data);
    }

    free(data);
}

void ColorMap::setColors(const Colors &colors) {
    m_colors = colors;
}

void ColorMap::setColor(const Color &color, const glm::ivec2 &uv) {
    m_colors[color] = uv;
}

const ColorMap::Colors& ColorMap::getColors() const {
    return m_colors;
}

const glm::ivec2& ColorMap::getUV(const Color &color) const {
    return m_colors.at(color);
}

const Color& ColorMap::getColor(const glm::ivec2 &uv) const {
    for (const auto &p : m_colors) {
        if (p.second == uv) {
            return p.first;
        }
    }

    throw std::runtime_error("Color at " + glm::to_string(uv) + " not found");
}

glm::vec2 ColorMap::getNormalizedUV(const Color &color) const {
    return normalizeUV(getUV(color));
}

glm::vec2 ColorMap::normalizeUV(const glm::ivec2 &uv) const {
    return {
        (float) uv.x / (float) getWidth(),
        (float) uv.y / (float) getHeight()
    };
}

glm::ivec2 ColorMap::denormalizeUV(const glm::vec2 &uv) const {
    return {
        static_cast<int>(uv.x * (float) getWidth()),
        static_cast<int>(uv.y * (float) getHeight())
    };
}

glm::vec2 ColorMap::getCenterOffset() const {
    return {
        (1.0f / (float) getWidth()) / 2,
        (1.0f / (float) getHeight()) / 2
    };
}

void ColorMap::setWidth(uint width) {
    m_texture->setWidth(width);
}

void ColorMap::setHeight(uint height) {
    m_texture->setHeight(height);
}

void ColorMap::setFormat(uint format) {
    m_texture->setFormat(format);
}

uint ColorMap::getWidth() const {
    return m_texture->getWidth();
}

uint ColorMap::getHeight() const {
    return m_texture->getHeight();
}

uint ColorMap::getFormat() const {
    return m_texture->getFormat();
}

bool ColorMap::exists(const Color &color) const {
    return m_colors.find(color) != m_colors.end();
}

bool ColorMap::exists(const glm::ivec2 &uv) const {
    return std::any_of(m_colors.begin(), m_colors.end(), [&](const auto &p) { return p.second == uv; });
}

const Texture2DPtr& ColorMap::get() const {
    return m_texture;
}
}
