#include <algine/core/texture/Texture.h>
#include <algine/core/Engine.h>

#include <iostream>

#include "core/IOStreamUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "internal/SOP.h"

using namespace algine;
using namespace std;

#ifdef ALGINE_SECURE_OPERATIONS
#include "internal/SOPConstants.h"
#define SOP_BOUND_PTR getBoundTexture(m_target)
#define SOP_OBJECT_TYPE getTextureObject(m_target)
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME getTextureObjectName(m_target)

inline void* getBoundTexture(const uint target) {
    switch (target) {
        case GL_TEXTURE_2D:
            return Engine::getBoundTexture2D();
        case GL_TEXTURE_CUBE_MAP:
            return Engine::getBoundTextureCube();
        default:
            assert(0);
    }
}

inline uint getTextureObject(const uint target) {
    switch (target) {
        case GL_TEXTURE_2D:
            return SOPConstants::Texture2DObject;
        case GL_TEXTURE_CUBE_MAP:
            return SOPConstants::TextureCubeObject;
        default:
            assert(0);
    }
}

inline string getTextureObjectName(const uint target) {
    switch (target) {
        case GL_TEXTURE_2D:
            return SOPConstants::Texture2DStr;
        case GL_TEXTURE_CUBE_MAP:
            return SOPConstants::TextureCubeStr;
        default:
            assert(0);
    }
}
#endif

namespace algine {
Texture::Texture() {
    glGenTextures(1, &m_id);
}

Texture::Texture(uint target): Texture() {
    m_target = target;
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

void Texture::bind() const {
    commitBinding()
    glBindTexture(m_target, m_id);
}

void Texture::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindTexture(m_target, 0);
}

void Texture::use(uint slot) const {
    commitBinding()
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_target, m_id);
}

void Texture::setParams(const map<uint, uint> &params) {
    checkBinding()

    for (const auto & key : params) {
        glTexParameteri(m_target, key.first, key.second);
    }
}

void Texture::setParams(const map<uint, float> &params) {
    checkBinding()

    for (const auto & key : params) {
        glTexParameterf(m_target, key.first, key.second);
    }
}

void Texture::applyTextureCreateInfo(const TextureCreateInfo &createInfo) {
    m_lod = createInfo.lod;
    m_format = createInfo.format;
    m_width = createInfo.width;
    m_height = createInfo.height;

    bind();
    setParams(createInfo.params);
    update();
    unbind();
}

void Texture::setLOD(uint lod) {
    m_lod = lod;
}

void Texture::setFormat(uint format) {
    m_format = format;
}

void Texture::setWidth(uint width) {
    m_width = width;
}

void Texture::setHeight(uint height) {
    m_height = height;
}

void Texture::setDimensions(uint width, uint height) {
    m_width = width;
    m_height = height;
}

uint Texture::getLOD() const {
    return m_lod;
}

uint Texture::getFormat() const {
    return m_format;
}

uint Texture::getWidth() const {
    return m_width;
}

uint Texture::getHeight() const {
    return m_height;
}

uint Texture::getId() const {
    return m_id;
}

uint Texture::getParam(uint param) const {
    checkBinding()

    int p = 0;
    glGetTexParameteriv(m_target, param, &p);

    return p;
}

void Texture::activateSlot(uint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

Texture::FormatInfo Texture::getFormatInfo(uint format) {
    switch (format) {
        case Texture::DepthComponent16:
            return {Texture::DepthComponent, DataType::UnsignedShort};
        case Texture::DepthComponent24:
            return {Texture::DepthComponent, DataType::UnsignedInt};
        case Texture::DepthComponent32F:
            return {Texture::DepthComponent, DataType::Float};
        case Texture::Depth24Stencil8:
            return {Texture::DepthStencil, static_cast<DataType>(GL_UNSIGNED_INT_24_8)};
        case Texture::Depth32FStencil8:
            return {Texture::DepthStencil, static_cast<DataType>(GL_FLOAT_32_UNSIGNED_INT_24_8_REV)};
        case Texture::RGB:
            return {Texture::RGB, DataType::UnsignedByte};
        case Texture::RGBA:
            return {Texture::RGBA, DataType::UnsignedByte};
        case Texture::Red8:
            return {Texture::Red, DataType::UnsignedByte};
        case Texture::Red16F:
            return {Texture::Red, DataType::HalfFloat};
        case Texture::Red32F:
            return {Texture::Red, DataType::Float};
        case Texture::RG8:
            return {Texture::RG, DataType::UnsignedByte};
        case Texture::RG16F:
            return {Texture::RG, DataType::HalfFloat};
        case Texture::RG32F:
            return {Texture::RG, DataType::Float};
        case Texture::RGB8:
            return {Texture::RGB, DataType::UnsignedByte};
        case Texture::RGB16F:
            return {Texture::RGB, DataType::HalfFloat};
        case Texture::RGB32F:
            return {Texture::RGB, DataType::Float};
        case Texture::RGBA8:
            return {Texture::RGBA, DataType::UnsignedByte};
        case Texture::RGBA16F:
            return {Texture::RGBA, DataType::HalfFloat};
        case Texture::RGBA32F:
            return {Texture::RGBA, DataType::Float};
        case Texture::Red8UI:
            return {Texture::RedInteger, DataType::UnsignedByte};
        case Texture::RG8UI:
            return {Texture::RGInteger, DataType::UnsignedByte};
        case Texture::RGB8UI:
            return {Texture::RGBInteger, DataType::UnsignedByte};
        case Texture::RGBA8UI:
            return {Texture::RGBAInteger, DataType::UnsignedByte};
        case Texture::Red8I:
            return {Texture::RedInteger, DataType::Byte};
        case Texture::RG8I:
            return {Texture::RGInteger, DataType::Byte};
        case Texture::RGB8I:
            return {Texture::RGBInteger, DataType::Byte};
        case Texture::RGBA8I:
            return {Texture::RGBAInteger, DataType::Byte};
        case Texture::Red16UI:
            return {Texture::RedInteger, DataType::UnsignedShort};
        case Texture::RG16UI:
            return {Texture::RGInteger, DataType::UnsignedShort};
        case Texture::RGB16UI:
            return {Texture::RGBInteger, DataType::UnsignedShort};
        case Texture::RGBA16UI:
            return {Texture::RGBAInteger, DataType::UnsignedShort};
        case Texture::Red16I:
            return {Texture::RedInteger, DataType::Short};
        case Texture::RG16I:
            return {Texture::RGInteger, DataType::Short};
        case Texture::RGB16I:
            return {Texture::RGBInteger, DataType::Short};
        case Texture::RGBA16I:
            return {Texture::RGBAInteger, DataType::Short};
        case Texture::Red32UI:
            return {Texture::RedInteger, DataType::UnsignedInt};
        case Texture::RG32UI:
            return {Texture::RGInteger, DataType::UnsignedInt};
        case Texture::RGB32UI:
            return {Texture::RGBInteger, DataType::UnsignedInt};
        case Texture::RGBA32UI:
            return {Texture::RGBAInteger, DataType::UnsignedInt};
        case Texture::Red32I:
            return {Texture::RedInteger, DataType::Int};
        case Texture::RG32I:
            return {Texture::RGInteger, DataType::Int};
        case Texture::RGB32I:
            return {Texture::RGBInteger, DataType::Int};
        case Texture::RGBA32I:
            return {Texture::RGBAInteger, DataType::Int};
        default: {
            throw std::runtime_error("Unsupported internal format " + to_string(format));
        }
    }
}

void Texture::texFromFile(uint target, const TextureFileInfo &info) { // TODO: TextureFileInfo remove 'dataType': it is always UnsignedByte
    stbi_set_flip_vertically_on_load(info.flip);

    auto bytes = IOStreamUtils::readAll<stbi_uc>(info.path, info.ioSystem);
    int channels;

    unsigned char *data = stbi_load_from_memory(
            bytes.array(), (int) bytes.size(), reinterpret_cast<int*>(&m_width), reinterpret_cast<int*>(&m_height), &channels, 0);

    int formats[] = {Red, RG, RGB, RGBA};
    int dataFormat = formats[channels - 1];

    m_format = dataFormat;

    if (data) {
        glTexImage2D(target, m_lod, m_format, m_width, m_height, 0, dataFormat, static_cast<uint>(info.dataType), data);
        glGenerateMipmap(m_target);
    } else {
        cerr << "Failed to load texture " << info.path << "\n";
        return;
    }

    stbi_image_free(data);
}
}
