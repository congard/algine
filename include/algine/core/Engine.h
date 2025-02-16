#ifndef ALGINE_ENGINE_H
#define ALGINE_ENGINE_H

#include <algine/gpu/context/Context.h>
#include <algine/core/types.h>
#include <algine/io/IOSystem.h>

#include <tulz/observer/Subject.h>
#include <glm/vec2.hpp>

#include <string>
#include <memory>

#ifdef ALGINE_QT_PLATFORM
    #include <QApplication>
#endif

namespace algine {
class AL_EXPORT Engine {
    friend class GLFWWindow;
    friend class QtWindow;

public:
    enum class GraphicsAPI {
        Core,
        ES
    };

public:
    using DestroySubject = tulz::Subject<>;

public:
    static void init(int argc, char *const *argv);
    static void init();
    static void wait();
    static void destroy();

    /**
     * Calls <code>Engine::init</code> -> <code>func</code> ->
     * <code>Engine::wait</code> -> <code>Engine::destroy</code>
     * @param argc
     * @param argv
     * @param func
     */
    static void exec(int argc, char *const *argv, const std::function<void()> &func);

    static void exec(const std::function<void()> &func);

    /**
     * Adds an onDestroy listener `observer`.
     * @param observer The listener.
     * @return Subscription.
     * @note This function can be safely called during
     * static initialization.
     */
    static DestroySubject::Subscription_t addOnDestroyListener(DestroySubject::ObserverAutoPtr_t observer);

    static void setDefaultIOSystem(IOSystem *ioSystem);
    static void setDefaultIOSystem(const std::shared_ptr<IOSystem> &ioSystem);
    static const std::shared_ptr<IOSystem>& getDefaultIOSystem();

    static void setAPIVersion(int version, GraphicsAPI api);

    static int getAPIVersion();
    static GraphicsAPI getGraphicsAPI();

    // TODO: move to Locale?
    static void setLanguage(std::string_view isoCode);
    static void setCountry(std::string_view isoCode);

    static const std::string& getLanguage();
    static const std::string& getCountry();

    static const Context& getApplicationContext();

    static std::string getGPUVendor();
    static std::string getGPURenderer();

    static uint getError();

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
    // TODO: move to GraphicsBackend
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
    static void setViewport(glm::ivec2 viewport);

    static void waitForExecution();
    static void flushCommandBuffer();

public:
    static int64_t time();
    static int64_t timeFromStart();

#ifdef __ANDROID__
public:
    class Android {
    public:
        static std::string getAppDataDirectory();
    };
#endif

private:
    static DestroySubject& get_onDestroy();

private:
    static std::shared_ptr<IOSystem> m_defaultIOSystem;

    static int m_apiVersion;
    static GraphicsAPI m_graphicsAPI;

    static std::string m_languageCode;
    static std::string m_countryCode;

    static Context m_appContext;

    static int m_argc;
    static char **m_argv;

#ifdef ALGINE_QT_PLATFORM
    static QApplication *m_qApp;
#endif

private:
    static int64_t m_startTime;
};
}

#endif //ALGINE_ENGINE_H
