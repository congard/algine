#include <algine/core/texture/Texture.h>
#include <algine/core/Engine.h>

#include <iostream>

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

#include "core/IOStreamUtils.h"

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

void Texture::activateSlot(uint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

void Texture::texFromFile(uint target, const TextureFileInfo &info) {
    stbi_set_flip_vertically_on_load(info.flip);

    auto bytes = IOStreamUtils::readAll<stbi_uc>(info.path, info.ioSystem);
    int channels;

    unsigned char *data = stbi_load_from_memory(
            bytes.array(), (int) bytes.size(), reinterpret_cast<int*>(&m_width), reinterpret_cast<int*>(&m_height), &channels, 0);

    int formats[] = {Red, RG, RGB, RGBA};
    int dataFormat = formats[channels - 1];

    enable_if_android(
        if (channels < 3) {
            m_format = RGB;
        } else {
            m_format = dataFormat;
        }
    )

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
