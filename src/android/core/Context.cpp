#include <algine/core/Context.h>
#include <algine/core/ContextConfig.h>

#include "core/ContextCommon.h"

#include <EGL/egl.h>

namespace algine {
bool Context::createImpl(int major, int minor, const ContextConfig &config) {
    auto &debugWriter = Engine::getDebugWriter();

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

    return true;
}

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
}
