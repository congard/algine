#include <algine/core/context/Context.h>
#include <algine/core/context/ContextConfig.h>

#include "core/context/ContextCommon.h"

#include <GLFW/glfw3.h>

namespace algine {
bool Context::createImpl(int major, int minor, const ContextConfig &config) {
    auto &debugWriter = Engine::getDebugWriter();

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugWriter != nullptr);

    // nContext - native context
    GLFWwindow* nContext = glfwCreateWindow(
        config.width, config.height, "",
        nullptr, static_cast<GLFWwindow*>(config.parent.m_context)
    );

    if (!nContext) {
        printContextCreationErrorMessage();
        return false;
    }

    m_context = nContext;

    return true;
}

bool Context::destroy() {
    if (isInitialized()) {
        glfwDestroyWindow(static_cast<GLFWwindow*>(m_context));
        auto error = glfwGetError(nullptr);

        if (!error) {
            getOnDestroy().notify(this);
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

void* Context::getNative() const {
    return m_context;
}

Context Context::getCurrent() {
    return {getCurrentNative()};
}

void* Context::getCurrentNative() {
    return glfwGetCurrentContext();
}
}
