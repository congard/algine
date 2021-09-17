#include <algine/core/Context.h>
#include <algine/core/ContextConfig.h>
#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>

#include "core/debug/Debug.h"

#include <thread>

#ifndef __ANDROID__
    #include <GLFW/glfw3.h>
#else
    #include <EGL/egl.h>
#endif

#ifdef ALGINE_SECURE_OPERATIONS
    #define sop_context_destroyed(context) Engine::contextDestroyed(context)
#else
    #define sop_context_destroyed(context)
#endif

namespace algine {
inline void printContextNotInitializedError() {
    Log::error("Context", "Context has not been initialized yet");
}

bool Context::create(const ContextConfig &config) {
    if (!Engine::getApplicationContext().m_context) {
        Log::error("Context::create", "Application context hasn't been initialized yet");
        return false;
    }

    auto printContextCreationErrorMessage = []() {
        Log::error("Context::create", "Context creation failed");
    };

    const auto apiVersion = Engine::getAPIVersion();

    int majorVersion = apiVersion / 100;
    int minorVersion = (apiVersion - majorVersion * 100) / 10;

    auto &debugWriter = Engine::getDebugWriter();

#ifndef __ANDROID__
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugWriter != nullptr);

    // nContext - native context
    GLFWwindow* nContext = glfwCreateWindow(config.width, config.height, "",
            nullptr, static_cast<GLFWwindow*>(config.parent.m_context));

    if (!nContext) {
        printContextCreationErrorMessage();
        return false;
    }

    m_context = nContext;
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
        Log::error("Context::create", "No config found");
        return false;
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

    EGLContext nContext = eglCreateContext(dpy, eglConfig, config.parent.m_context, ctxAttrib);

    if (!nContext) {
        printContextCreationErrorMessage();
        return false;
    }

    m_context = nContext;
    m_display = dpy;
    m_surface = surf;
#endif

    return true;
}

bool Context::create(const Context &parent) {
    return create(ContextConfig {parent});
}

bool Context::create() {
    return create(ContextConfig {});
}

inline void enableDebugOutputIfPossible() {
    if (auto &debugWriter = Engine::getDebugWriter(); debugWriter != nullptr) {
        debugWriter->logger() << "Context " << Context::getCurrentNative() << " now is current for thread "
            << std::hash<std::thread::id>{}(std::this_thread::get_id());

        enableDebugOutput();
    }
}

#ifndef __ANDROID__
bool Context::destroy() {
    if (isInitialized()) {
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_context));
        auto error = glfwGetError(nullptr);

        if (!error) {
            sop_context_destroyed(m_context);
            m_context = nullptr;
        }

        return !error;
    } else {
        return false;
    }
}

bool Context::makeCurrent() const {
    if (!isInitialized()) {
        printContextNotInitializedError();
        return false;
    } else {
        glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_context));
        enableDebugOutputIfPossible();
        return true;
    }
}

bool Context::detach() const {
    glfwMakeContextCurrent(nullptr);
    return true;
}

bool Context::isCurrent() const {
    return getCurrentNative() == m_context;
}

bool Context::isInitialized() const {
    return m_context;
}

Context Context::getCurrent() {
    return {getCurrentNative()};
}

void* Context::getCurrentNative() {
    return glfwGetCurrentContext();
}
#else
bool Context::destroy() {
    auto ctx = m_context;
    auto dpy = m_display;
    auto surf = m_surface;

    if (!isInitialized()) {
        Log::error("Context::destroy") <<
            "Context is not initialized:\n"
            "context is " << ctx << "\n"
            "display is " << dpy << "\n"
            "surface is " << surf;
        return false;
    }

    detach();

    bool r_destroySurf = eglDestroySurface(dpy, surf);
    bool r_destroyCtx  = eglDestroyContext(dpy, ctx);
    bool r_terminate   = eglTerminate(dpy);

    if (r_destroyCtx) {
        sop_context_destroyed(ctx);
    }

    if (!r_destroySurf || !r_destroyCtx || !r_terminate) {
        Log::error("Context::destroy") <<
            "The context is not completely destroyed:\n"
            "surface destroyed: " << r_destroySurf << "\n"
            "context destroyed: " << r_destroyCtx << "\n"
            "display terminated: " << r_terminate;
        return false;
    }

    m_context = nullptr;
    m_display = nullptr;
    m_surface = nullptr;

    return true;
}

bool Context::makeCurrent() const {
    if (!isInitialized()) {
        printContextNotInitializedError();
        return false;
    } else {
        auto result = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
        if (result) enableDebugOutputIfPossible();
        return result;
    }
}

bool Context::detach() const {
    return eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

bool Context::isCurrent() const {
    return getCurrentNative() == m_context;
}

bool Context::isInitialized() const {
    return m_context && m_display && m_surface;
}

Context Context::getCurrent() {
    return {
        eglGetCurrentContext(),
        eglGetCurrentDisplay(),
        eglGetCurrentSurface(EGL_READ) // read and write should be the same
    };
}

void* Context::getCurrentNative() {
    return eglGetCurrentContext();
}
#endif
}
