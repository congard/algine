#include <algine/core/texture/Texture2D.h>

#include <algine/core/Engine.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTexture2D()
#define SOP_OBJECT_TYPE SOPConstants::Texture2DObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::Texture2DStr
#include "../SOP.h"
#include "../SOPConstants.h"

#include "../PublicObjectTools.h"

using namespace std;

namespace algine {
vector<Texture2DPtr> Texture2D::publicTextures;

Texture2D::Texture2D()
    : Texture(GL_TEXTURE_2D)
{
    // see initializer list above
}

void Texture2D::fromFile(const std::string &path, DataType dataType, bool flipImage) {
    checkBinding()
    texFromFile(path, GL_TEXTURE_2D, dataType, flipImage);
}

void Texture2D::update() {
    // last 3 params never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    checkBinding()

    // GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
    // GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    uint dataFormat = m_format == DepthComponent ? DepthComponent : Red;

    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, GL_BYTE, nullptr);
}

void Texture2D::update(const uint dataFormat, const uint dataType, const void *const data) {
    checkBinding()
    glTexImage2D(m_target, m_lod, m_format, m_width, m_height, 0, dataFormat, dataType, data);
}

map<uint, uint> Texture2D::defaultParams() {
    return {
        {MinFilter, Linear},
        {MagFilter, Linear},
        {WrapU, ClampToEdge},
        {WrapV, ClampToEdge}
    };
}

Texture2DPtr Texture2D::getByName(const string &name) {
    return PublicObjectTools::getByName(publicTextures, name);
}

Texture2D* Texture2D::byName(const string &name) {
    return PublicObjectTools::byName(publicTextures, name);
}
}