#include <algine/core/texture/Texture.h>

#include <algine/core/Engine.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "../SOP.h"

using namespace algine;
using namespace std;

#ifdef ALGINE_SECURE_OPERATIONS
#include "../SOPConstants.h"
#define SOP_BOUND_PTR getBoundTexture(target)
#define SOP_OBJECT_TYPE getTextureObject(target)
#define SOP_OBJECT_ID id
#define SOP_OBJECT_NAME getTextureObjectName(target)

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
    glGenTextures(1, &id);
}

Texture::Texture(const uint target): Texture() {
    this->target = target;
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind() const {
    commitBinding()
    glBindTexture(target, id);
}

void Texture::use(const uint slot) const {
    commitBinding()
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, id);
}

void Texture::setParams(const map<uint, uint> &params) {
    checkBinding()
    for(const auto & key : params)
        glTexParameteri(target, key.first, key.second);
}

void Texture::setParams(const map<uint, float> &params) {
    checkBinding()
    for(const auto & key : params)
        glTexParameterf(target, key.first, key.second);
}

void Texture::applyTextureCreateInfo(const TextureCreateInfo &createInfo) {
    lod = createInfo.lod;
    format = createInfo.format;
    width = createInfo.width;
    height = createInfo.height;

    bind();
    setParams(createInfo.params);
    update();
    unbind();
}

void Texture::setLOD(const uint _lod) {
    lod = _lod;
}

void Texture::setFormat(const uint _format) {
    format = _format;
}

void Texture::setWidth(const uint _width) {
    width = _width;
}

void Texture::setHeight(const uint _height) {
    height = _height;
}

void Texture::setWidthHeight(const uint _width, const uint _height) {
    width = _width;
    height = _height;
}

void Texture::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindTexture(target, 0);
}

uint Texture::getLOD() const {
    return lod;
}

uint Texture::getFormat() const {
    return format;
}

uint Texture::getWidth() const {
    return width;
}

uint Texture::getHeight() const {
    return height;
}

uint Texture::getId() const {
    return id;
}

void Texture::texFromFile(const string &path, uint _target, uint dataType, bool flipImage) {
    int channels;
    stbi_set_flip_vertically_on_load(flipImage);
    unsigned char *data = stbi_load(path.c_str(),
            reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height), &channels, 0);

    int formats[] = {Red, RG, RGB, RGBA};
    int dataFormat = formats[channels - 1];

    if (data) {
        glTexImage2D(_target, lod, format, width, height, 0, dataFormat, dataType, data);
        glGenerateMipmap(target);
    } else {
        cerr << "Failed to load texture " << path << "\n";
        return;
    }

    stbi_image_free(data);
}
}
