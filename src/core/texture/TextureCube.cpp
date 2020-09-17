#include <algine/core/texture/TextureCube.h>

#include <algine/core/Engine.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTextureCube()
#define SOP_OBJECT_TYPE SOPConstants::TextureCubeObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::TextureCubeStr
#include "../SOP.h"
#include "../SOPConstants.h"

#include "../PublicObjectTools.h"

using namespace std;

namespace algine {
vector<TextureCubePtr> TextureCube::publicObjects;

TextureCube::TextureCube()
    : Texture(GL_TEXTURE_CUBE_MAP)
{
    // see initializer list above
}

void TextureCube::fromFile(const std::string &path, Face face, DataType dataType, bool flipImage) {
    checkBinding()
    texFromFile(path, static_cast<uint>(face), dataType, flipImage);
}

void TextureCube::update() {
    // last 3 params never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    checkBinding()

    // GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
    // GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    uint dataFormat = m_format == DepthComponent ? DepthComponent : Red;

    for (uint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_lod, m_format, m_width, m_height, 0, dataFormat, GL_BYTE, nullptr);
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

TextureCubePtr TextureCube::getByName(const string &name) {
    return PublicObjectTools::getByName(publicObjects, name);
}

TextureCube* TextureCube::byName(const string &name) {
    return PublicObjectTools::byName(publicObjects, name);
}
}