#include <algine/core/texture/TextureCube.h>

#include <map>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;

namespace algine {
vector<TextureCubePtr> TextureCube::publicObjects;

AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(TextureCube) {
    .obj = []() {
        auto tex = new TextureCube(std::false_type {});
        tex->m_id = 0;
        tex->m_target = GL_TEXTURE_CUBE_MAP;
        return tex;
    }()
};

TextureCube::TextureCube()
    : Texture(GL_TEXTURE_CUBE_MAP) {}

void TextureCube::fromFile(Face face, const TextureFileInfo &fileInfo) {
    checkBinding();
    texFromFile(static_cast<uint>(face), fileInfo);
}

void TextureCube::fromFile(Face face, const std::string &path) {
    fromFile(face, TextureFileInfo {path});
}

void TextureCube::update() {
    checkBinding();

    auto [dataFormat, dataType] = Texture::getFormatInfo(m_format);

    for (uint i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_lod, m_format, m_width, m_height, 0, dataFormat,
                     static_cast<GLenum>(dataType), nullptr);
    }
}

map<uint, uint> TextureCube::defaultParams() {
    return {
        {MinFilter, Linear},
        {MagFilter, Linear},
        {WrapU, ClampToEdge},
        {WrapV, ClampToEdge},
        {WrapW, ClampToEdge}
    };
}

// TextureCube faces have the same internal format, size, mipmap level
// https://www.khronos.org/opengl/wiki/Cubemap_Texture
#define target static_cast<uint>(Face::Right)

uint TextureCube::getActualFormat() const {
    return getTexParam(target, GL_TEXTURE_INTERNAL_FORMAT);
}

uint TextureCube::getActualWidth() const {
    return getTexParam(target, GL_TEXTURE_WIDTH);
}

uint TextureCube::getActualHeight() const {
    return getTexParam(target, GL_TEXTURE_HEIGHT);
}

#undef target

TextureCubePtr TextureCube::getByName(const string &name) {
    return PublicObjectTools::getByName<TextureCubePtr>(name);
}

TextureCube* TextureCube::byName(const string &name) {
    return PublicObjectTools::byName<TextureCube>(name);
}
}
