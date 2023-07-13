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
    #include "../../src/android/core/Bridge.h"
#endif

#ifdef ALGINE_SECURE_OPERATIONS
    #include "internal/SOPConstants.h"
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

Lua Engine::m_lua;

tulz::Subject<> Engine::m_onDestroy;

int Engine::m_argc;
char **Engine::m_argv;

#ifdef ALGINE_QT_PLATFORM
QApplication *Engine::m_qApp;
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

void Engine::init(int argc, char *const *argv) {
    m_argc = argc;
    m_argv = new char*[argc];

    for (int i = 0; i < argc; ++i)
        m_argv[i] = strdup(argv[i]);

#ifdef ALGINE_QT_PLATFORM
    // https://doc.qt.io/qt-5/qapplication.html#QApplication
    m_qApp = new QApplication(m_argc, m_argv);
    QApplication::setAttribute(Qt::AA_DontCheckOpenGLContextThreadAffinity);
#endif

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
    m_defaultShaderProgram->m_id = 0;

    m_defaultInputLayout = (InputLayout*) malloc(sizeof(InputLayout));
    m_defaultInputLayout->m_id = 0;

#ifndef __ANDROID__
    m_defaultIOSystem = make_shared<StandardIOSystem>();

    tulz::LocaleInfo::Info localeInfo;

    // if LC_ALL is not defined, std::locale("") will throw an exception
    if (is_linux() && !getenv("LC_ALL")) {
        localeInfo = tulz::LocaleInfo::get("en_GB.UTF-8");
        Log::error("Algine", "LC_ALL is not defined, locale was set to en_GB.UTF-8");
    } else {
        localeInfo = tulz::LocaleInfo::get(locale("").name().c_str());
    }

    setLanguage(localeInfo.languageCode);
    setCountry(localeInfo.countryCode);
#else
    m_defaultIOSystem = make_shared<AssetsIOSystem>();
    m_appContext = Context::getCurrent();
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

void Engine::init() {
    std::string workingDir = tulz::Path::getWorkingDirectory().toString();
    int argc = 1;
    char *argv[] = {const_cast<char*>(workingDir.c_str())};
    init(argc, argv);
}

void Engine::wait() {
#ifdef ALGINE_QT_PLATFORM
    QApplication::exec();
#endif
}

void Engine::destroy() {
    m_onDestroy.notify();

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

#ifndef ALGINE_QT_PLATFORM
    // Terminate GLFW, clearing any resources allocated by GLFW
    enable_if_desktop(glfwTerminate());
#else
    // QApplication must be destroyed before arguments become invalid
    // https://doc.qt.io/qt-5/qapplication.html#QApplication
    delete m_qApp;
#endif

    for (int i = 0; i < m_argc; ++i)
        free(m_argv[i]); // string was allocated using malloc by strdup
    delete[] m_argv;
}

void Engine::exec(int argc, char *const *argv, const std::function<void()> &func) {
    init(argc, argv);
    func();
    wait();
    destroy();
}

void Engine::exec(const std::function<void()> &func) {
    std::string workingDir = tulz::Path::getWorkingDirectory().toString();
    int argc = 1;
    char *argv[] = {const_cast<char*>(workingDir.c_str())};
    exec(argc, argv, func);
}

tulz::Subscription<> Engine::addOnDestroyListener(const tulz::Observer<> &observer) {
    return m_onDestroy.subscribe(observer);
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

Lua& Engine::getLua() {
    if (!m_lua.isInitialized())
        m_lua.init();
    return m_lua;
}

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
        if (auto obj = boundObj.read(Context::getCurrentNative()); obj) return obj; \
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

void Engine::waitForExecution() {
    glFinish();
}

void Engine::flushCommandBuffer() {
    glFlush();
}

long Engine::time() {
    using namespace chrono;

    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

long Engine::timeFromStart() {
    return Engine::time() - m_startTime;
}

#ifdef __ANDROID__
std::string Engine::Android::getAppDataDirectory() {
    return AndroidBridge::getAppDataDirectory();
}
#endif

#ifdef ALGINE_SECURE_OPERATIONS
void Engine::setBoundObject(uint type, const void *obj) {
    auto ctx = Context::getCurrentNative();

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
