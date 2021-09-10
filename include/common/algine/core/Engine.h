#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <algine/core/debug/DebugWriter.h>
#include <algine/core/io/IOSystem.h>
#include <algine/types.h>

#include <string>
#include <memory>

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
    friend class Font;

public:
    enum class GraphicsAPI {
        Core,
        ES
    };

public:
    static void init();
    static void destroy();

    static void setDebugWriter(DebugWriter *debugWriter);
    static std::unique_ptr<DebugWriter>& getDebugWriter();

    static void setDefaultIOSystem(IOSystem *ioSystem);
    static void setDefaultIOSystem(const std::shared_ptr<IOSystem> &ioSystem);
    static const std::shared_ptr<IOSystem>& getDefaultIOSystem();

    static void setAPIVersion(int version, GraphicsAPI api);

    static int getAPIVersion();
    static GraphicsAPI getGraphicsAPI();

    static void setDPI(uint dpi);
    static uint getDPI();

    static void setLanguage(std::string_view isoCode);
    static void setCountry(std::string_view isoCode);

    static const std::string& getLanguage();
    static const std::string& getCountry();

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
    enum class DepthTest {
        Never = 0x0200,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class FaceCulling {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };

    enum class PolyType {
        Line = 0x1,
        Triangle = 0x4,
        TriangleStrip = 0x5,
        TriangleFan = 0x6
    };

public:
    static void enableDepthTest();
    static void enableFaceCulling();
    static void enableDepthMask();

    static void disableDepthTest();
    static void disableFaceCulling();
    static void disableDepthMask();

    static bool isDepthTestEnabled();
    static bool isFaceCullingEnabled();
    static bool isDepthMaskEnabled();

    static void drawElements(uint start, uint count, PolyType polyType = PolyType::Triangle);
    static void drawArrays(uint start, uint count, PolyType polyType = PolyType::Triangle);

    static void setDepthTestMode(DepthTest mode);
    static void setFaceCullingMode(FaceCulling mode);
    static void setViewport(int x, int y, int width, int height);
    static void setViewport(int width, int height);

public:
    static long time();
    static long timeFromStart();

private:
    static std::unique_ptr<DebugWriter> m_debugWriter;
    static std::shared_ptr<IOSystem> m_defaultIOSystem;
    static void *m_fontLibrary;

    static int m_apiVersion;
    static GraphicsAPI m_graphicsAPI;

    static uint m_dpi;

    static std::string m_languageCode;
    static std::string m_countryCode;

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
