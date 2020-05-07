#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <string>

namespace algine {
class Framebuffer;
class Texture2D;
class TextureCube;

class Engine {
    friend class Framebuffer;
    friend class Texture;
    friend class Texture2D;
    friend class TextureCube;

public:
    static void init();
    static void destroy();

    static Framebuffer* getBoundFramebuffer();
    static Texture2D* getBoundTexture2D();
    static TextureCube* getBoundTextureCube();

    static std::string getGPUVendor();
    static std::string getGPURenderer();

public:
    static inline Framebuffer* defaultFramebuffer() {
        return m_defaultFramebuffer;
    }

    static inline Texture2D* defaultTexture2D() {
        return m_defaultTexture2D;
    }

    static inline TextureCube* defaultTextureCube() {
        return m_defaultTextureCube;
    }

private:
    static Framebuffer *m_defaultFramebuffer;
    static Texture2D *m_defaultTexture2D;
    static TextureCube *m_defaultTextureCube;

private:
    static void setBoundObject(uint type, const void *obj);

private:
    static Framebuffer *m_boundFramebuffer;
    static Texture2D *m_boundTexture2D;
    static TextureCube *m_boundTextureCube;
};
}

#endif //ALGINE_ENGINE_H
