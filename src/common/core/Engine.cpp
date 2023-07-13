#include <algine/core/Engine.h>

#include <algine/templates.h>
#include <algine/core/TypeRegistry.h>
#include <algine/core/log/Log.h>

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/LinearLayout.h>
#include <algine/core/widgets/RelativeLayout.h>
#include <algine/core/widgets/Label.h>
#include <algine/core/widgets/ImageWidget.h>

#include <algine/gl.h>

#include <chrono>

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

// TODO: move to static initializers
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

int Engine::m_apiVersion;
Engine::GraphicsAPI Engine::m_graphicsAPI;

uint Engine::m_dpi = 96;

std::string Engine::m_languageCode;
std::string Engine::m_countryCode;

Context Engine::m_appContext;

tulz::Subject<> Engine::m_onDestroy;

int Engine::m_argc;
char **Engine::m_argv;

#ifdef ALGINE_QT_PLATFORM
QApplication *Engine::m_qApp;
#endif

long Engine::m_startTime;

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

    // TODO: move to static initializers
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

    // TODO: move to TypeRegistry's static initializer
    TypeRegistry::clear();

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

string Engine::getGPUVendor() {
    return reinterpret_cast<char const*>(glGetString(GL_VENDOR));
}

string Engine::getGPURenderer() {
    return reinterpret_cast<char const*>(glGetString(GL_RENDERER));
}

uint Engine::getError() {
    return glGetError();
}

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
}
