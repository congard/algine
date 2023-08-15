#include <algine/core/context/Context.h>
#include <algine/core/context/ContextConfig.h>

#include "core/context/ContextCommon.h"

#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <QOpenGLContext>

namespace algine {
#define LOG_TAG "Context"
#define qContext static_cast<QOpenGLContext*>(m_context)
#define qSurface static_cast<QOffscreenSurface*>(m_surface)

bool Context::createImpl(int major, int minor, const ContextConfig &config) {
    QSurfaceFormat format;
    format.setVersion(major, minor);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setOption(QSurfaceFormat::FormatOption::DebugContext, Log::isEnabled(Logger::Type::Debug));

    auto surface = new QOffscreenSurface();
    surface->setFormat(format);
    surface->create();

    auto context = new QOpenGLContext();
    context->setFormat(format);

    if (config.parent.isInitialized())
        context->setShareContext(static_cast<QOpenGLContext*>(config.parent.m_context));

    if (!context->create()) {
        printContextCreationErrorMessage();
        return false;
    }

    m_context = context;
    m_surface = surface;

    return true;
}

bool Context::destroy() {
    if (isInitialized()) {
        delete qContext;
        delete qSurface;

        getOnDestroy().notify(this);

        m_context = nullptr;
        m_surface = nullptr;

        return true;
    } else {
        return false;
    }
}

bool Context::makeCurrent() const {
    if (!isInitialized()) {
        printContextNotInitializedError();
        return false;
    } else {
        Log::debug(LOG_TAG) << "Making context " << this << " current" << Log::endl;

        bool result = qContext->makeCurrent(qSurface);

        Log::debug(LOG_TAG) << "Context " << this << " (native=" << getNative() << ") has been made current" << Log::endl;

        if (result)
            enableDebugOutputIfPossible();

        Log::debug(LOG_TAG) << "Context " << this << " is ready" << Log::endl;

        return result;
    }
}

bool Context::detach() const {
    Log::debug(LOG_TAG) << "Detaching context " << this << Log::endl;

    auto res = qContext->makeCurrent(nullptr);

    Log::debug(LOG_TAG) << "Context " << this << " has been successfully detached" << Log::endl;

    return res;
}

bool Context::isCurrent() const {
    return getCurrentNative() == m_context;
}

bool Context::isInitialized() const {
    return m_context;
}

void* Context::getNative() const {
    return m_context;
}

Context Context::getCurrent() {
    return {
        .m_context = getCurrentNative(),
        .m_surface = QOpenGLContext::currentContext()->surface()
    };
}

void* Context::getCurrentNative() {
    return QOpenGLContext::currentContext();
}
}
