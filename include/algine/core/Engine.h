#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <string>

namespace algine {
class Framebuffer;
class Renderbuffer;
class Texture2D;
class TextureCube;

class Engine {
    friend class Framebuffer;
    friend class Renderbuffer;
    friend class Texture;
    friend class Texture2D;
    friend class TextureCube;

public:
    static void init();
    static void destroy();

    static Framebuffer* getBoundFramebuffer();
    static Renderbuffer* getBoundRenderbuffer();
    static Texture2D* getBoundTexture2D();
    static TextureCube* getBoundTextureCube();

    static std::string getGPUVendor();
    static std::string getGPURenderer();

public:
    static inline Framebuffer* defaultFramebuffer() {
        return m_defaultFramebuffer;
    }

    static inline Renderbuffer* defaultRenderbuffer() {
        return m_defaultRenderbuffer;
    }

    static inline Texture2D* defaultTexture2D() {
        return m_defaultTexture2D;
    }

    static inline TextureCube* defaultTextureCube() {
        return m_defaultTextureCube;
    }

private:
    static Framebuffer *m_defaultFramebuffer;
    static Renderbuffer *m_defaultRenderbuffer;
    static Texture2D *m_defaultTexture2D;
    static TextureCube *m_defaultTextureCube;

private:
    static void setBoundObject(uint type, const void *obj);

private:
    static Framebuffer *m_boundFramebuffer;
    static Renderbuffer *m_boundRenderbuffer;
    static Texture2D *m_boundTexture2D;
    static TextureCube *m_boundTextureCube;
};
}

#endif //ALGINE_ENGINE_H
