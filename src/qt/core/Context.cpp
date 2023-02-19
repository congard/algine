#include <algine/core/Context.h>

#include "core/ContextCommon.h"

#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <QOpenGLContext>

namespace algine {
#define qContext static_cast<QOpenGLContext*>(m_context)
#define qSurface static_cast<QOffscreenSurface*>(m_surface)

bool Context::createImpl(int major, int minor, const ContextConfig &config) {
    auto &debugWriter = Engine::getDebugWriter();

    QSurfaceFormat format;
    format.setVersion(major, minor);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setOption(QSurfaceFormat::FormatOption::DebugContext, debugWriter != nullptr);

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

        sop_context_destroyed(m_context);

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
        bool result = qContext->makeCurrent(qSurface);
        if (result)
            enableDebugOutputIfPossible();
        return result;
    }
}

bool Context::detach() const {
    return qContext->makeCurrent(nullptr);
}

bool Context::isCurrent() const {
    return getCurrentNative() == m_context;
}

bool Context::isInitialized() const {
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
