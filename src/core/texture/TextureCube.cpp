#include <algine/core/texture/TextureCube.h>

#include <algine/core/Engine.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTextureCube()
#define SOP_OBJECT_TYPE SOPConstants::TextureCubeObject
#define SOP_OBJECT_ID id
#define SOP_OBJECT_NAME SOPConstants::TextureCubeStr
#include "../SOP.h"
#include "../SOPConstants.h"

using namespace std;

namespace algine {
TextureCube::TextureCube(): Texture(GL_TEXTURE_CUBE_MAP) {}

void TextureCube::fromFile(const std::string &path, uint face, uint dataType, bool flipImage) {
    checkBinding()
    texFromFile(path, face, dataType, flipImage);
}

// GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
// GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
#define _findCorrectDataFormat \
uint dataFormat = Red; \
if (format == DepthComponent) \
    dataFormat = DepthComponent;

void TextureCube::update() {
    // last 3 params never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    checkBinding()
    _findCorrectDataFormat
    for (uint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lod, format, width, height, 0, dataFormat, GL_BYTE, nullptr);
}

map<uint, uint> TextureCube::defaultParams() {
    return map<uint, uint> {
        pair<uint, uint> {MinFilter, Linear},
        pair<uint, uint> {MagFilter, Linear},
        pair<uint, uint> {WrapU, ClampToEdge},
        pair<uint, uint> {WrapV, ClampToEdge},
        pair<uint, uint> {WrapW, ClampToEdge}
    };
}
}