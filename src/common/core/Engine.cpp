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
#include <algine/core/log/Log.h>

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/LinearLayout.h>
#include <algine/core/widgets/RelativeLayout.h>
#include <algine/core/widgets/Label.h>
#include <algine/core/widgets/ImageWidget.h>

#include <algine/gl.h>

#include <chrono>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "core/debug/Debug.h"

#ifndef __ANDROID__
    #include <algine/core/io/StandardIOSystem.h>
    #include <GLFW/glfw3.h>
    #include <tulz/LocaleInfo.h>
    #include <locale>
#else
    #include <algine/core/io/AssetsIOSystem.h>
    #include <EGL/egl.h>
#endif

#ifdef ALGINE_SECURE_OPERATIONS
    #include "internal/SOPConstants.h"

    #define sop_context_destroyed(context) contextDestroyed(context)
#else
    #define sop_context_destroyed(context)
#endif

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
unique_ptr<DebugWriter> Engine::m_debugWriter;
shared_ptr<IOSystem> Engine::m_defaultIOSystem;
void* Engine::m_fontLibrary;

int Engine::m_apiVersion;
Engine::GraphicsAPI Engine::m_graphicsAPI;

uint Engine::m_dpi = 96;

std::string Engine::m_languageCode;
std::string Engine::m_countryCode;

Context Engine::m_appContext;

#ifndef __ANDROID__
std::mutex Engine::m_contextCreationMutex;
std::mutex Engine::m_contextDestructionMutex;
#endif

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

string exec(const char *cmd) {
    string result;
    FILE* pipe = popen(cmd, "r");

    if (!pipe)
        throw runtime_error("popen() failed!");

    try {
        char buffer[256];

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }

    pclose(pipe);

    return result;
}

void linux_detect_dpi() {
    auto dpi_str = exec("xrdb -query | grep 'Xft.dpi' | cut -f 2");

    if (auto pos = dpi_str.find('\n'); pos != string::npos)
        dpi_str.erase(pos);

    char *end;

    if (auto dpi = strtol(dpi_str.c_str(), &end, 0); *end == '\0') {
        Engine::setDPI(dpi);
    }
}

void Engine::init() {
    enable_if_desktop(
        m_apiVersion = 400;
        m_graphicsAPI = GraphicsAPI::Core;
    )

    enable_if_android(
        m_apiVersion = 300;
        m_graphicsAPI = GraphicsAPI::ES;
    )

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

#ifndef __ANDROID__
    m_defaultIOSystem = make_shared<StandardIOSystem>();

    auto localeInfo = tulz::LocaleInfo::get(locale("").name().c_str());
    setLanguage(localeInfo.languageCode);
    setCountry(localeInfo.countryCode);
#else
    m_defaultIOSystem = make_shared<AssetsIOSystem>();
    m_appContext = getCurrentContext();
#endif

    FT_Init_FreeType(reinterpret_cast<FT_Library *>(&m_fontLibrary));

    alRegisterType(Container);
    alRegisterType(LinearLayout);
    alRegisterType(RelativeLayout);
    alRegisterType(Label);
    alRegisterType(ImageWidget);

#if defined(__linux__) && !defined(__ANDROID__)
    linux_detect_dpi();
#endif

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

    FT_Done_FreeType(static_cast<FT_Library>(m_fontLibrary));
    m_fontLibrary = nullptr;

    enable_if_desktop(glfwTerminate()); // Terminate GLFW, clearing any resources allocated by GLFW
}

void Engine::setDebugWriter(DebugWriter *debugWriter) {
    m_debugWriter.reset(debugWriter);

    enable_if_android(enableDebugOutput());
}

unique_ptr<DebugWriter>& Engine::getDebugWriter() {
    return m_debugWriter;
}

void Engine::setDefaultIOSystem(IOSystem *ioSystem) {
    m_defaultIOSystem.reset(ioSystem);
}

void Engine::setDefaultIOSystem(const shared_ptr<IOSystem> &ioSystem) {
    m_defaultIOSystem = ioSystem;
}

const shared_ptr<IOSystem>& Engine::getDefaultIOSystem() {
    return m_defaultIOSystem;
}

void Engine::setAPIVersion(int version, GraphicsAPI api) {
    m_apiVersion = version;
    m_graphicsAPI = api;
}

int Engine::getAPIVersion() {
    return m_apiVersion;
}

Engine::GraphicsAPI Engine::getGraphicsAPI() {
    return m_graphicsAPI;
}

void Engine::setDPI(uint dpi) {
    m_dpi = dpi;
}

uint Engine::getDPI() {
    return m_dpi;
}

void Engine::setLanguage(std::string_view isoCode) {
    m_languageCode = isoCode;
}

void Engine::setCountry(std::string_view isoCode) {
    m_countryCode = isoCode;
}

const std::string& Engine::getLanguage() {
    return m_languageCode;
}

const std::string& Engine::getCountry() {
    return m_countryCode;
}

const Context& Engine::getApplicationContext() {
    return m_appContext;
}

Context Engine::createContext(const ContextConfig &config) {
    if (!m_appContext.context)
        throw runtime_error("Application context hasn't been initialized yet");

    auto printContextCreationErrorMessage = []() {
        Log::error("Engine::createContext") << "Context creation failed" << Log::end;
    };

    Context context;

    int majorVersion = m_apiVersion / 100;
    int minorVersion = (m_apiVersion - majorVersion * 100) / 10;

    auto &debugWriter = Engine::getDebugWriter();

#ifndef __ANDROID__
    m_contextCreationMutex.lock(); // without locking, segfault may occur (tested on X11)

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugWriter != nullptr);

    // nContext - native context
    GLFWwindow* nContext = glfwCreateWindow(config.width, config.height, "",
            nullptr, static_cast<GLFWwindow*>(config.parent.context));

    if (!nContext) {
        printContextCreationErrorMessage();
        return {};
    }

    glfwMakeContextCurrent(nContext);

    m_contextCreationMutex.unlock();

    context.context = nContext;
#else
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    int vers[2];
    eglInitialize(dpy, &vers[0], &vers[1]);

    // select egl config

    int configAttr[] = {
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            EGL_LEVEL, 0,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_NONE
    };

    EGLConfig eglConfig;
    int numConfig;

    eglChooseConfig(dpy, configAttr, &eglConfig, 1, &numConfig);

    if (numConfig == 0) {
        Log::error("Engine::createContext") << "No config found" << Log::end;
        return {};
    }

    // create surface

    int surfAttr[] = {
            EGL_WIDTH, config.width,
            EGL_HEIGHT, config.height,
            EGL_NONE
    };

    EGLSurface surf = eglCreatePbufferSurface(dpy, eglConfig, surfAttr);

    // create context

    int ctxAttrib[] = {
            EGL_CONTEXT_MAJOR_VERSION, majorVersion,
            EGL_CONTEXT_MINOR_VERSION, minorVersion,
            EGL_NONE
    };

    EGLContext nContext = eglCreateContext(dpy, eglConfig, config.parent.context, ctxAttrib);

    if (!nContext) {
        printContextCreationErrorMessage();
        return {};
    }

    eglMakeCurrent(dpy, surf, surf, nContext);

    context.context = nContext;
    context.display = dpy;
    context.surface = surf;
#endif

    if (debugWriter != nullptr) {
        debugWriter->begin();

        auto &stream = debugWriter->stream();
        stream << '[' << Engine::time() << "] Context " << nContext << " has been initialized";

        debugWriter->end();

        enableDebugOutput();
    }

    return context;
}

#ifndef __ANDROID__
inline auto curr_ctx() {
    return glfwGetCurrentContext();
}

Context Engine::getCurrentContext() {
    return {curr_ctx()};
}

bool Engine::destroyContext(const Context &context) {
    if (context.context) {
        m_contextDestructionMutex.lock();

        glfwDestroyWindow(static_cast<GLFWwindow*>(context.context));
        auto error = glfwGetError(nullptr);

        m_contextDestructionMutex.unlock();

        if (!error) {
            sop_context_destroyed(context.context);
        }

        return !error;
    } else {
        return false;
    }
}
#else
inline auto curr_ctx() {
    return eglGetCurrentContext();
}

Context Engine::getCurrentContext() {
    return {curr_ctx()};
}

bool Engine::destroyContext(const Context &context) {
    auto ctx = context.context;
    auto dpy = context.display;
    auto surf = context.surface;

    if (!ctx || !dpy || !surf) {
        Log::error("Engine::destroyContext") <<
            "Context is incomplete:\n"
            "context is " << ctx << "\n"
            "display is " << dpy << "\n"
            "surface is " << surf << "\n"
            "Note: you can call Engine::destroyContext only for contexts returned by Engine::createContext"
        << Log::end;
        return false;
    }

    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    bool r_destroySurf = eglDestroySurface(dpy, surf);
    bool r_destroyCtx  = eglDestroyContext(dpy, ctx);
    bool r_terminate   = eglTerminate(dpy);

    if (r_destroyCtx) {
        sop_context_destroyed(ctx);
    }

    if (!r_destroySurf || !r_destroyCtx || !r_terminate) {
        Log::error("Engine::destroyContext") <<
            "The context is not completely destroyed:\n"
            "surface destroyed: " << r_destroySurf << "\n"
            "context destroyed: " << r_destroyCtx << "\n"
            "display terminated: " << r_terminate << Log::end;
        return false;
    }

    return true;
}
#endif

#ifdef ALGINE_SECURE_OPERATIONS
Engine::ContextObjectMap<Framebuffer> Engine::m_boundFramebuffer;
Engine::ContextObjectMap<Renderbuffer> Engine::m_boundRenderbuffer;
Engine::ContextObjectMap<Texture2D> Engine::m_boundTexture2D;
Engine::ContextObjectMap<TextureCube> Engine::m_boundTextureCube;
Engine::ContextObjectMap<ArrayBuffer> Engine::m_boundArrayBuffer;
Engine::ContextObjectMap<IndexBuffer> Engine::m_boundIndexBuffer;
Engine::ContextObjectMap<UniformBuffer> Engine::m_boundUniformBuffer;
Engine::ContextObjectMap<ShaderProgram> Engine::m_boundShaderProgram;
Engine::ContextObjectMap<InputLayout> Engine::m_boundInputLayout;

#define returnBound(type, name, boundObj, defaultObj) \
    type* Engine::name() {                            \
        if (auto obj = boundObj.read(curr_ctx()); obj) return obj; \
        else return defaultObj; \
    }

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

bool Engine::isDepthTestEnabled() {
    return glIsEnabled(GL_DEPTH_TEST);
}

bool Engine::isFaceCullingEnabled() {
    return glIsEnabled(GL_CULL_FACE);
}

bool Engine::isDepthMaskEnabled() {
    GLboolean enabled;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);
    return enabled;
}

void Engine::drawElements(uint start, uint count, PolyType polyType) {
    glDrawElements(static_cast<GLenum>(polyType), count, GL_UNSIGNED_INT, reinterpret_cast<void*>(start * sizeof(uint)));
}

void Engine::drawArrays(uint start, uint count, PolyType polyType) {
    glDrawArrays(static_cast<GLenum>(polyType), start, count);
}

void Engine::setDepthTestMode(DepthTest mode) {
    glDepthFunc(static_cast<GLenum>(mode));
}

void Engine::setFaceCullingMode(FaceCulling mode) {
    glCullFace(static_cast<GLenum>(mode));
}

void Engine::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Engine::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}

long Engine::time() {
    using namespace chrono;

    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

long Engine::timeFromStart() {
    return Engine::time() - m_startTime;
}

#ifdef ALGINE_SECURE_OPERATIONS
void Engine::setBoundObject(uint type, const void *obj) {
    auto ctx = curr_ctx();

    switch (type) {
        case SOPConstants::FramebufferObject:
            m_boundFramebuffer.write(ctx, (Framebuffer*) obj);
            break;
        case SOPConstants::RenderbufferObject:
            m_boundRenderbuffer.write(ctx, (Renderbuffer*) obj);
            break;
        case SOPConstants::Texture2DObject:
            m_boundTexture2D.write(ctx, (Texture2D*) obj);
            break;
        case SOPConstants::TextureCubeObject:
            m_boundTextureCube.write(ctx, (TextureCube*) obj);
            break;
        case SOPConstants::ArrayBufferObject:
            m_boundArrayBuffer.write(ctx, (ArrayBuffer*) obj);
            break;
        case SOPConstants::IndexBufferObject:
            m_boundIndexBuffer.write(ctx, (IndexBuffer*) obj);
            break;
        case SOPConstants::UniformBufferObject:
            m_boundUniformBuffer.write(ctx, (UniformBuffer*) obj);
            break;
        case SOPConstants::ShaderProgramObject:
            m_boundShaderProgram.write(ctx, (ShaderProgram*) obj);
            break;
        case SOPConstants::InputLayoutObject:
            m_boundInputLayout.write(ctx, (InputLayout*) obj);
            break;
        default:
            break;
    }
}

void Engine::contextDestroyed(void *context) {
    m_boundFramebuffer.erase(context);
    m_boundRenderbuffer.erase(context);
    m_boundTexture2D.erase(context);
    m_boundTextureCube.erase(context);
    m_boundArrayBuffer.erase(context);
    m_boundIndexBuffer.erase(context);
    m_boundUniformBuffer.erase(context);
    m_boundShaderProgram.erase(context);
    m_boundInputLayout.erase(context);
}
#endif
}
