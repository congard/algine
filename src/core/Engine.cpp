#include <algine/core/Engine.h>

#include <algine/core/Framebuffer.h>
#include <algine/texture/Texture2D.h>
#include <algine/texture/TextureCube.h>

#include <GL/glew.h>

using namespace std;

namespace algine {
Framebuffer* Engine::m_defaultFramebuffer;
Texture2D* Engine::m_defaultTexture2D;
TextureCube* Engine::m_defaultTextureCube;

Framebuffer* Engine::m_boundFramebuffer;
Texture2D* Engine::m_boundTexture2D;
TextureCube* Engine::m_boundTextureCube;

void Engine::init() {
    // We use malloc instead of new since we don't want the ctor to be
    // called because ctor generates new texture id. We don't need it.
    // In the case of increasing the number of operations performed by
    // the ctor, create something like init() in the following classes
    m_defaultTexture2D = (Texture2D*) malloc(sizeof(Texture2D));
    m_defaultTexture2D->id = 0;
    m_defaultTexture2D->target = GL_TEXTURE_2D;

    m_defaultTextureCube = (TextureCube*) malloc(sizeof(TextureCube));
    m_defaultTextureCube->id = 0;
    m_defaultTextureCube->target = GL_TEXTURE_CUBE_MAP;

    m_defaultFramebuffer = (Framebuffer*) malloc(sizeof(Framebuffer));
    m_defaultFramebuffer->m_id = 0;
}

void Engine::destroy() {
    Texture2D::destroy(m_defaultTexture2D);
    TextureCube::destroy(m_defaultTextureCube);
    Framebuffer::destroy(m_defaultFramebuffer);
}

#ifdef ALGINE_SECURE_OPERATIONS
Framebuffer* Engine::getBoundFramebuffer() {
    return m_boundFramebuffer == nullptr ? m_defaultFramebuffer : m_boundFramebuffer;
}

Texture2D* Engine::getBoundTexture2D() {
    return m_boundTexture2D == nullptr ? m_defaultTexture2D : m_boundTexture2D;
}

TextureCube* Engine::getBoundTextureCube() {
    return m_boundTextureCube == nullptr ? m_defaultTextureCube : m_boundTextureCube;
}
#else
#define retnull(type, name) type* Engine::name() { return nullptr; }

retnull(Framebuffer, getBoundFramebuffer)
retnull(Texture2D, getBoundTexture2D)
retnull(TextureCube, getBoundTextureCube)
#endif

string Engine::getGPUVendor() {
    return reinterpret_cast<char const*>(glGetString(GL_VENDOR));
}

string Engine::getGPURenderer() {
    return reinterpret_cast<char const*>(glGetString(GL_RENDERER));
}

#ifdef ALGINE_SECURE_OPERATIONS
#include "SOPConstants.h"

void Engine::setBoundObject(uint type, const void *obj) {
    switch (type) {
        case SOPConstants::FramebufferObject:
            m_boundFramebuffer = (Framebuffer*) obj;
            break;
        case SOPConstants::Texture2DObject:
            m_boundTexture2D = (Texture2D*) obj;
            break;
        case SOPConstants::TextureCubeObject:
            m_boundTextureCube = (TextureCube*) obj;
            break;
        default:
            break;
    }
}

#endif
}
