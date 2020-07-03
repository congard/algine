#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <string>

namespace algine {
class Framebuffer;
class Renderbuffer;
class Texture2D;
class TextureCube;
class ArrayBuffer;
class IndexBuffer;
class ShaderProgram;

class Engine {
    friend class Framebuffer;
    friend class Renderbuffer;
    friend class Texture;
    friend class Texture2D;
    friend class TextureCube;
    friend class Buffer;
    friend class ShaderProgram;

public:
    static void init();
    static void destroy();

    static Framebuffer* getBoundFramebuffer();
    static Renderbuffer* getBoundRenderbuffer();
    static Texture2D* getBoundTexture2D();
    static TextureCube* getBoundTextureCube();
    static ArrayBuffer* getBoundArrayBuffer();
    static IndexBuffer* getBoundIndexBuffer();
    static ShaderProgram* getBoundShaderProgram();

    static std::string getGPUVendor();
    static std::string getGPURenderer();

public:
    static Framebuffer* defaultFramebuffer();
    static Renderbuffer* defaultRenderbuffer();
    static Texture2D* defaultTexture2D();
    static TextureCube* defaultTextureCube();
    static ArrayBuffer* defaultArrayBuffer();
    static IndexBuffer* defaultIndexBuffer();
    static ShaderProgram *defaultShaderProgram();

private:
    static Framebuffer *m_defaultFramebuffer;
    static Renderbuffer *m_defaultRenderbuffer;
    static Texture2D *m_defaultTexture2D;
    static TextureCube *m_defaultTextureCube;
    static ArrayBuffer *m_defaultArrayBuffer;
    static IndexBuffer *m_defaultIndexBuffer;
    static ShaderProgram *m_defaultShaderProgram;

private:
    static void setBoundObject(uint type, const void *obj);

private:
    static Framebuffer *m_boundFramebuffer;
    static Renderbuffer *m_boundRenderbuffer;
    static Texture2D *m_boundTexture2D;
    static TextureCube *m_boundTextureCube;
    static ArrayBuffer *m_boundArrayBuffer;
    static IndexBuffer *m_boundIndexBuffer;
    static ShaderProgram *m_boundShaderProgram;
};
}

#endif //ALGINE_ENGINE_H
