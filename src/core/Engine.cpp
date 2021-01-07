#include <algine/core/Engine.h>

#include <algine/core/Framebuffer.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/buffers/IndexBuffer.h>
#include <algine/core/buffers/UniformBuffer.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/InputLayout.h>
#include <algine/core/TypeRegistry.h>

#include <GLFW/glfw3.h>

#include <chrono>

#ifndef ALGINE_CORE_ONLY
#include <algine/std/model/Shape.h>
#include <algine/std/model/Model.h>

inline void initExtra() {
    using namespace algine;

    alRegisterType(Shape);
    alRegisterType(Model);
}

#else
#define initExtra()
#endif

using namespace std;

namespace algine {
long Engine::m_startTime;

Framebuffer* Engine::m_defaultFramebuffer;
Renderbuffer* Engine::m_defaultRenderbuffer;
Texture2D* Engine::m_defaultTexture2D;
TextureCube* Engine::m_defaultTextureCube;
ArrayBuffer* Engine::m_defaultArrayBuffer;
IndexBuffer* Engine::m_defaultIndexBuffer;
UniformBuffer* Engine::m_defaultUniformBuffer;
ShaderProgram* Engine::m_defaultShaderProgram;
InputLayout* Engine::m_defaultInputLayout;

Framebuffer* Engine::m_boundFramebuffer;
Renderbuffer* Engine::m_boundRenderbuffer;
Texture2D* Engine::m_boundTexture2D;
TextureCube* Engine::m_boundTextureCube;
ArrayBuffer* Engine::m_boundArrayBuffer;
IndexBuffer* Engine::m_boundIndexBuffer;
UniformBuffer* Engine::m_boundUniformBuffer;
ShaderProgram* Engine::m_boundShaderProgram;
InputLayout* Engine::m_boundInputLayout;

void Engine::init() {
    m_startTime = Engine::time();

    // We use malloc instead of new since we don't want the ctor to be
    // called because ctor generates new texture id. We don't need it.
    // In the case of increasing the number of operations performed by
    // the ctor, create something like init() in the following classes
    m_defaultTexture2D = (Texture2D*) malloc(sizeof(Texture2D));
    m_defaultTexture2D->m_id = 0;
    m_defaultTexture2D->m_target = GL_TEXTURE_2D;

    m_defaultTextureCube = (TextureCube*) malloc(sizeof(TextureCube));
    m_defaultTextureCube->m_id = 0;
    m_defaultTextureCube->m_target = GL_TEXTURE_CUBE_MAP;

    m_defaultFramebuffer = (Framebuffer*) malloc(sizeof(Framebuffer));
    m_defaultFramebuffer->m_id = 0;

    m_defaultRenderbuffer = (Renderbuffer*) malloc(sizeof(Renderbuffer));
    m_defaultRenderbuffer->m_id = 0;

    m_defaultArrayBuffer = (ArrayBuffer*) malloc(sizeof(ArrayBuffer));
    m_defaultArrayBuffer->m_id = 0;
    m_defaultArrayBuffer->m_target = GL_ARRAY_BUFFER;

    m_defaultIndexBuffer = (IndexBuffer*) malloc(sizeof(IndexBuffer));
    m_defaultIndexBuffer->m_id = 0;
    m_defaultIndexBuffer->m_target = GL_ELEMENT_ARRAY_BUFFER;

    m_defaultUniformBuffer = (UniformBuffer*) malloc(sizeof(UniformBuffer));
    m_defaultUniformBuffer->m_id = 0;
    m_defaultUniformBuffer->m_target = GL_UNIFORM_BUFFER;

    m_defaultShaderProgram = (ShaderProgram*) malloc(sizeof(ShaderProgram));
    m_defaultShaderProgram->id = 0;

    m_defaultInputLayout = (InputLayout*) malloc(sizeof(InputLayout));
    m_defaultInputLayout->m_id = 0;

    initExtra();
}

void Engine::destroy() {
    // destructor is not called since we do not need to delete
    // default OpenGL objects (with id 0)
    free(m_defaultFramebuffer);
    free(m_defaultRenderbuffer);
    free(m_defaultTexture2D);
    free(m_defaultTextureCube);
    free(m_defaultArrayBuffer);
    free(m_defaultIndexBuffer);
    free(m_defaultUniformBuffer);
    free(m_defaultShaderProgram);
    free(m_defaultInputLayout);

    TypeRegistry::clear();

    glfwTerminate(); // Terminate GLFW, clearing any resources allocated by GLFW
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
returnBound(UniformBuffer, getBoundUniformBuffer, m_boundUniformBuffer, m_defaultUniformBuffer)
returnBound(ShaderProgram, getBoundShaderProgram, m_boundShaderProgram, m_defaultShaderProgram)
returnBound(InputLayout, getBoundInputLayout, m_boundInputLayout, m_defaultInputLayout)
#else
#define returnNull(type, name) type* Engine::name() { return nullptr; }

returnNull(Framebuffer, getBoundFramebuffer)
returnNull(Renderbuffer, getBoundRenderbuffer)
returnNull(Texture2D, getBoundTexture2D)
returnNull(TextureCube, getBoundTextureCube)
returnNull(ArrayBuffer, getBoundArrayBuffer)
returnNull(IndexBuffer, getBoundIndexBuffer)
returnNull(UniformBuffer, getBoundUniformBuffer)
returnNull(ShaderProgram, getBoundShaderProgram)
returnNull(InputLayout, getBoundInputLayout)
#endif

string Engine::getGPUVendor() {
    return reinterpret_cast<char const*>(glGetString(GL_VENDOR));
}

string Engine::getGPURenderer() {
    return reinterpret_cast<char const*>(glGetString(GL_RENDERER));
}

uint Engine::getError() {
    return glGetError();
}

#define returnDefault(type, name, obj) type* Engine::name() { return obj; }

returnDefault(Framebuffer, defaultFramebuffer, m_defaultFramebuffer)
returnDefault(Renderbuffer, defaultRenderbuffer, m_defaultRenderbuffer)
returnDefault(Texture2D, defaultTexture2D, m_defaultTexture2D)
returnDefault(TextureCube, defaultTextureCube, m_defaultTextureCube)
returnDefault(ArrayBuffer, defaultArrayBuffer, m_defaultArrayBuffer)
returnDefault(IndexBuffer, defaultIndexBuffer, m_defaultIndexBuffer)
returnDefault(UniformBuffer, defaultUniformBuffer, m_defaultUniformBuffer)
returnDefault(ShaderProgram, defaultShaderProgram, m_defaultShaderProgram)
returnDefault(InputLayout, defaultInputLayout, m_defaultInputLayout)

void Engine::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void Engine::enableFaceCulling() {
    glEnable(GL_CULL_FACE);
}

void Engine::enableDepthMask() {
    glDepthMask(true);
}

void Engine::disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

void Engine::disableFaceCulling() {
    glDisable(GL_CULL_FACE);
}

void Engine::disableDepthMask() {
    glDepthMask(false);
}

void Engine::drawElements(uint start, uint count, uint polyType) {
    glDrawElements(polyType, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(start * sizeof(uint)));
}

void Engine::setDepthTestMode(uint mode) {
    glDepthFunc(mode);
}

void Engine::setFaceCullingMode(uint mode) {
    glCullFace(mode);
}

void Engine::setViewport(uint width, uint height, uint x, uint y) {
    glViewport(x, y, width, height);
}

long Engine::time() {
    using namespace chrono;

    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

long Engine::timeFromStart() {
    return Engine::time() - m_startTime;
}

#ifdef ALGINE_SECURE_OPERATIONS
#include "internal/SOPConstants.h"

void Engine::setBoundObject(uint type, const void *obj) {
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
        case SOPConstants::UniformBufferObject:
            m_boundUniformBuffer = (UniformBuffer*) obj;
            break;
        case SOPConstants::ShaderProgramObject:
            m_boundShaderProgram = (ShaderProgram*) obj;
            break;
        case SOPConstants::InputLayoutObject:
            m_boundInputLayout = (InputLayout*) obj;
            break;
        default:
            break;
    }
}

#endif
}
