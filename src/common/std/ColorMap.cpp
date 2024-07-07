#include <algine/std/ColorMap.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>

#include <stdexcept>

namespace algine {
bool ColorMap::ColorComparator::operator()(glm::ivec2 lhs, glm::ivec2 rhs) const {
    return std::hash<glm::ivec2>()(lhs) < std::hash<glm::ivec2>()(rhs);
}

ColorMap::ColorMap(Object *parent)
    : Object(parent),
      m_texture(nullptr) {}

ColorMap::ColorMap(uint width, uint height, uint format, Object *parent)
    : ColorMap(parent)
{
    create({width, height}, format);
}

ColorMap::ColorMap(glm::ivec2 size, uint format, Object *parent)
    : ColorMap(parent)
{
    create(size, format);
}

void ColorMap::create(glm::ivec2 size, uint format) {
    m_texture = new Texture2D(this);
    m_texture->bind();
    m_texture->setDimensions(size.x, size.y);
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
                auto &uv = p.first;
                auto &color = p.second;
                data[uv.y * width + uv.x] = color.red<T>();
            }
            break;
        case 2:
            for (const auto &p : colors) {
                auto &uv = p.first;
                auto &color = p.second;
                auto index = (uv.y * width + uv.x) * 2;
                data[index + 0] = color.red<T>();
                data[index + 1] = color.green<T>();
            }
            break;
        case 3:
            for (const auto &p : colors) {
                auto &uv = p.first;
                auto &color = p.second;
                auto index = (uv.y * width + uv.x) * 3;
                data[index + 0] = color.red<T>();
                data[index + 1] = color.green<T>();
                data[index + 2] = color.blue<T>();
            }

            break;
        case 4:
            for (const auto &p : colors) {
                auto &uv = p.first;
                auto &color = p.second;
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

void ColorMap::setColor(glm::ivec2 uv, const Color &color) {
    m_colors[uv] = color;
}

const ColorMap::Colors& ColorMap::getColors() const {
    return m_colors;
}

const glm::ivec2& ColorMap::getUV(const Color &color) const {
    for (const auto & [key, value] : m_colors) {
        if (value == color) {
            return key;
        }
    }

    throw std::runtime_error("ColorMap doesn't contain provided color");
}

const Color& ColorMap::getColor(glm::ivec2 uv) const {
    return m_colors.at(uv);
}

glm::vec2 ColorMap::getNormalizedUV(const Color &color) const {
    return normalizeUV(getUV(color));
}

glm::vec2 ColorMap::normalizeUV(glm::ivec2 uv) const {
    return normalizeUV(uv, {getWidth(), getHeight()});
}

glm::ivec2 ColorMap::denormalizeUV(glm::vec2 uv) const {
    return denormalizeUV(uv, {getWidth(), getHeight()});
}

glm::vec2 ColorMap::getCenterOffset() const {
    return getCenterOffset({getWidth(), getHeight()});
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
    return std::any_of(m_colors.begin(), m_colors.end(), [&](const auto &p) { return p.second == color; });
}

bool ColorMap::exists(glm::ivec2 uv) const {
    return m_colors.find(uv) != m_colors.end();
}

Texture2D* ColorMap::get() const {
    return m_texture;
}

glm::vec2 ColorMap::normalizeUV(glm::ivec2 uv, glm::ivec2 size) {
    return {
        (float) uv.x / (float) size.x,
        (float) uv.y / (float) size.y
    };
}

glm::ivec2 ColorMap::denormalizeUV(glm::vec2 uv, glm::ivec2 size) {
    return {
        static_cast<int>(uv.x * (float) size.x),
        static_cast<int>(uv.y * (float) size.y)
    };
}

glm::vec2 ColorMap::getCenterOffset(glm::ivec2 size) {
    return {
        (1.0f / (float) size.x) / 2,
        (1.0f / (float) size.y) / 2
    };
}
}
