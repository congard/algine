#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <algine/types.h>

#include <string>

namespace algine {
class Framebuffer;
class Renderbuffer;
class Texture2D;
class TextureCube;
class ArrayBuffer;
class IndexBuffer;
class UniformBuffer;
class ShaderProgram;
class InputLayout;

class Engine {
    friend class Framebuffer;
    friend class Renderbuffer;
    friend class Texture;
    friend class Texture2D;
    friend class TextureCube;
    friend class Buffer;
    friend class ShaderProgram;
    friend class InputLayout;

public:
    enum class GraphicsAPI {
        Core,
        ES
    };

public:
    static void init();
    static void destroy();

    static void setAPIVersion(int version, GraphicsAPI api);

    static int getAPIVersion();
    static GraphicsAPI getGraphicsAPI();

    static Framebuffer* getBoundFramebuffer();
    static Renderbuffer* getBoundRenderbuffer();
    static Texture2D* getBoundTexture2D();
    static TextureCube* getBoundTextureCube();
    static ArrayBuffer* getBoundArrayBuffer();
    static IndexBuffer* getBoundIndexBuffer();
    static UniformBuffer* getBoundUniformBuffer();
    static ShaderProgram* getBoundShaderProgram();
    static InputLayout* getBoundInputLayout();

    static std::string getGPUVendor();
    static std::string getGPURenderer();

    static uint getError();

public:
    static Framebuffer* defaultFramebuffer();
    static Renderbuffer* defaultRenderbuffer();
    static Texture2D* defaultTexture2D();
    static TextureCube* defaultTextureCube();
    static ArrayBuffer* defaultArrayBuffer();
    static IndexBuffer* defaultIndexBuffer();
    static UniformBuffer* defaultUniformBuffer();
    static ShaderProgram* defaultShaderProgram();
    static InputLayout* defaultInputLayout();

public:
    enum DepthTestModes {
        DepthTestNever = 0x0200,
        DepthTestLess,
        DepthTestEqual,
        DepthTestLessOrEqual,
        DepthTestGreater,
        DepthTestNotEqual,
        DepthTestGreaterOrEqual,
        DepthTestAlways
    };

    enum FaceCullingOptions {
        CullFaceFront = 0x0404,
        CullFaceBack = 0x0405,
        CullFaceFrontAndBack = 0x0408
    };

    enum PolyTypes {
        Triangle = 0x0004
    };

public:
    static void enableDepthTest();
    static void enableFaceCulling();
    static void enableDepthMask();

    static void disableDepthTest();
    static void disableFaceCulling();
    static void disableDepthMask();

    static void drawElements(uint start, uint count, uint polyType = Triangle);
    static void setDepthTestMode(uint mode);
    static void setFaceCullingMode(uint mode);
    static void setViewport(uint width, uint height, uint x = 0, uint y = 0);

public:
    static long time();
    static long timeFromStart();

private:
    static int m_apiVersion;
    static GraphicsAPI m_graphicsAPI;

private:
    static long m_startTime;

private:
    static Framebuffer *m_defaultFramebuffer;
    static Renderbuffer *m_defaultRenderbuffer;
    static Texture2D *m_defaultTexture2D;
    static TextureCube *m_defaultTextureCube;
    static ArrayBuffer *m_defaultArrayBuffer;
    static IndexBuffer *m_defaultIndexBuffer;
    static UniformBuffer *m_defaultUniformBuffer;
    static ShaderProgram *m_defaultShaderProgram;
    static InputLayout *m_defaultInputLayout;

private:
    static void setBoundObject(uint type, const void *obj);

private:
    static Framebuffer *m_boundFramebuffer;
    static Renderbuffer *m_boundRenderbuffer;
    static Texture2D *m_boundTexture2D;
    static TextureCube *m_boundTextureCube;
    static ArrayBuffer *m_boundArrayBuffer;
    static IndexBuffer *m_boundIndexBuffer;
    static UniformBuffer *m_boundUniformBuffer;
    static ShaderProgram *m_boundShaderProgram;
    static InputLayout *m_boundInputLayout;
};
}

#endif //ALGINE_ENGINE_H
