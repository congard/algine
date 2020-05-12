#include <algine/core/texture/Texture2D.h>

#include <algine/core/Engine.h>

#include <map>

#define SOP_BOUND_PTR Engine::getBoundTexture2D()
#define SOP_OBJECT_TYPE SOPConstants::Texture2DObject
#define SOP_OBJECT_ID id
#define SOP_OBJECT_NAME SOPConstants::Texture2DStr
#include "../SOP.h"
#include "../SOPConstants.h"

using namespace std;

namespace algine {
Texture2D::Texture2D(): Texture(GL_TEXTURE_2D) {}

void Texture2D::fromFile(const std::string &path, const uint dataType, const bool flipImage) {
    checkBinding()
    texFromFile(path, GL_TEXTURE_2D, dataType, flipImage);
}

// GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
// GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
#define _findCorrectDataFormat \
uint dataFormat = Red; \
if (format == DepthComponent) \
    dataFormat = DepthComponent;

void Texture2D::update() {
    // last 3 params never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    checkBinding()
    _findCorrectDataFormat
    glTexImage2D(target, lod, format, width, height, 0, dataFormat, GL_BYTE, nullptr);
}

void Texture2D::update(const uint dataFormat, const uint dataType, const void *const data) {
    checkBinding()
    glTexImage2D(target, lod, format, width, height, 0, dataFormat, dataType, data);
}

map<uint, uint> Texture2D::defaultParams() {
    return map<uint, uint> {
        pair<uint, uint> {MinFilter, Linear},
        pair<uint, uint> {MagFilter, Linear},
        pair<uint, uint> {WrapU, ClampToEdge},
        pair<uint, uint> {WrapV, ClampToEdge}
    };
}
}