#include <algine/core/texture/Texture2D.h>
#include <algine/core/Engine.h>

#include <tulz/Array.h>

#include <map>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;
using namespace tulz;

namespace algine {
vector<Texture2DPtr> Texture2D::publicObjects;

AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(Texture2D) {
    .obj = []() {
        auto tex = new Texture2D(std::false_type {});
        tex->m_id = 0;
        tex->m_target = GL_TEXTURE_2D;
        return tex;
    }()
};

Texture2D::Texture2D()
    : Texture(GL_TEXTURE_2D) {}

void Texture2D::fromFile(const TextureFileInfo &fileInfo) {
    checkBinding();
    texFromFile(GL_TEXTURE_2D, fileInfo);
}

void Texture2D::fromFile(const std::string &path) {
    fromFile(TextureFileInfo {path});
}

void Texture2D::update() {
    checkBinding();

    auto [dataFormat, dataType] = Texture::getFormatInfo(m_format);

    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, static_cast<GLenum>(dataType), nullptr);
}

void Texture2D::update(uint dataFormat, DataType dataType, const void *data) {
    checkBinding();
    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, static_cast<GLenum>(dataType), data);
}

map<uint, uint> Texture2D::defaultParams() {
    return {
        {MinFilter, Linear},
        {MagFilter, Linear},
        {WrapU, ClampToEdge},
        {WrapV, ClampToEdge}
    };
}

uint Texture2D::getActualFormat() const {
    checkBinding();
    return getTexParam(m_target, GL_TEXTURE_INTERNAL_FORMAT);
}

uint Texture2D::getActualWidth() const {
    checkBinding();
    return getTexParam(m_target, GL_TEXTURE_WIDTH);
}

uint Texture2D::getActualHeight() const {
    checkBinding();
    return getTexParam(m_target, GL_TEXTURE_HEIGHT);
}

Texture2DPtr Texture2D::getByName(const string &name) {
    return PublicObjectTools::getByName<Texture2DPtr>(name);
}

Texture2D* Texture2D::byName(const string &name) {
    return PublicObjectTools::byName<Texture2D>(name);
}
}
