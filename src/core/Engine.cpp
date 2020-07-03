#include <algine/core/Engine.h>

#include <algine/core/Framebuffer.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/buffers/IndexBuffer.h>
#include <algine/core/shader/ShaderProgram.h>

#include <GL/glew.h>

using namespace std;

namespace algine {
Framebuffer* Engine::m_defaultFramebuffer;
Renderbuffer* Engine::m_defaultRenderbuffer;
Texture2D* Engine::m_defaultTexture2D;
TextureCube* Engine::m_defaultTextureCube;
ArrayBuffer* Engine::m_defaultArrayBuffer;
IndexBuffer* Engine::m_defaultIndexBuffer;
ShaderProgram* Engine::m_defaultShaderProgram;

Framebuffer* Engine::m_boundFramebuffer;
Renderbuffer* Engine::m_boundRenderbuffer;
Texture2D* Engine::m_boundTexture2D;
TextureCube* Engine::m_boundTextureCube;
ArrayBuffer* Engine::m_boundArrayBuffer;
IndexBuffer* Engine::m_boundIndexBuffer;
ShaderProgram* Engine::m_boundShaderProgram;

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

    m_defaultRenderbuffer = (Renderbuffer*) malloc(sizeof(Renderbuffer));
    m_defaultRenderbuffer->id = 0;

    m_defaultArrayBuffer = (ArrayBuffer*) malloc(sizeof(ArrayBuffer));
    m_defaultArrayBuffer->m_id = 0;
    m_defaultArrayBuffer->m_target = GL_ARRAY_BUFFER;

    m_defaultIndexBuffer = (IndexBuffer*) malloc(sizeof(IndexBuffer));
    m_defaultIndexBuffer->m_id = 0;
    m_defaultIndexBuffer->m_target = GL_ELEMENT_ARRAY_BUFFER;

    m_defaultShaderProgram = (ShaderProgram*) malloc(sizeof(ShaderProgram));
    m_defaultShaderProgram->id = 0;
}

void Engine::destroy() {
    Framebuffer::destroy(m_defaultFramebuffer);
    Renderbuffer::destroy(m_defaultRenderbuffer);
    Texture2D::destroy(m_defaultTexture2D);
    TextureCube::destroy(m_defaultTextureCube);
    ArrayBuffer::destroy(m_defaultArrayBuffer);
    IndexBuffer::destroy(m_defaultIndexBuffer);
    ShaderProgram::destroy(m_defaultShaderProgram);
}

#ifdef ALGINE_SECURE_OPERATIONS
#define returnBound(type, name, boundObj, defaultObj) \
    type* Engine::name() { return boundObj == nullptr ? defaultObj : boundObj; }

returnBound(Framebuffer, getBoundFramebuffer, m_boundFramebuffer, m_defaultFramebuffer)
returnBound(Renderbuffer, getBoundRenderbuffer, m_boundRenderbuffer, m_defaultRenderbuffer)
returnBound(Texture2D, getBoundTexture2D, m_boundTexture2D, m_defaultTexture2D)
returnBound(TextureCube, getBoundTextureCube, m_boundTextureCube, m_defaultTextureCube)
returnBound(ArrayBuffer, getBoundArrayBuffer, m_boundArrayBuffer, m_defaultArrayBuffer)
returnBound(IndexBuffer, getBoundIndexBuffer, m_boundIndexBuffer, m_defaultIndexBuffer)
returnBound(ShaderProgram, getBoundShaderProgram, m_boundShaderProgram, m_defaultShaderProgram)
#else
#define returnNull(type, name) type* Engine::name() { return nullptr; }

returnNull(Framebuffer, getBoundFramebuffer)
returnNull(Renderbuffer, getBoundRenderbuffer)
returnNull(Texture2D, getBoundTexture2D)
returnNull(TextureCube, getBoundTextureCube)
returnNull(ArrayBuffer, getBoundArrayBuffer)
returnNull(IndexBuffer, getBoundIndexBuffer)
returnNull(ShaderProgram, getBoundShaderProgram)
#endif

string Engine::getGPUVendor() {
    return reinterpret_cast<char const*>(glGetString(GL_VENDOR));
}

string Engine::getGPURenderer() {
    return reinterpret_cast<char const*>(glGetString(GL_RENDERER));
}

#define returnDefault(type, name, obj) type* Engine::name() { return obj; }

returnDefault(Framebuffer, defaultFramebuffer, m_defaultFramebuffer)
returnDefault(Renderbuffer, defaultRenderbuffer, m_defaultRenderbuffer)
returnDefault(Texture2D, defaultTexture2D, m_defaultTexture2D)
returnDefault(TextureCube, defaultTextureCube, m_defaultTextureCube)
returnDefault(ArrayBuffer, defaultArrayBuffer, m_defaultArrayBuffer)
returnDefault(IndexBuffer, defaultIndexBuffer, m_defaultIndexBuffer)
returnDefault(ShaderProgram, defaultShaderProgram, m_defaultShaderProgram)

#ifdef ALGINE_SECURE_OPERATIONS
#include "SOPConstants.h"

void Engine::setBoundObject(const uint type, const void *const obj) {
    switch (type) {
        case SOPConstants::FramebufferObject:
            m_boundFramebuffer = (Framebuffer*) obj;
            break;
        case SOPConstants::RenderbufferObject:
            m_boundRenderbuffer = (Renderbuffer*) obj;
            break;
        case SOPConstants::Texture2DObject:
            m_boundTexture2D = (Texture2D*) obj;
            break;
        case SOPConstants::TextureCubeObject:
            m_boundTextureCube = (TextureCube*) obj;
            break;
        case SOPConstants::ArrayBufferObject:
            m_boundArrayBuffer = (ArrayBuffer*) obj;
            break;
        case SOPConstants::IndexBufferObject:
            m_boundIndexBuffer = (IndexBuffer*) obj;
            break;
        case SOPConstants::ShaderProgramObject:
            m_boundShaderProgram = (ShaderProgram*) obj;
            break;
        default:
            break;
    }
}

#endif
}
