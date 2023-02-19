#include <algine/core/Context.h>
#include <algine/core/ContextConfig.h>
#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>

namespace algine {
bool Context::create(const ContextConfig &config) {
    if (!Engine::getApplicationContext().m_context) {
        Log::error("Context::create", "Application context hasn't been initialized yet");
        return false;
    }

    const auto apiVersion = Engine::getAPIVersion();

    int majorVersion = apiVersion / 100;
    int minorVersion = (apiVersion - majorVersion * 100) / 10;

    return createImpl(majorVersion, minorVersion, config);
}

bool Context::create(const Context &parent) {
    return create(ContextConfig {parent});
}

bool Context::create() {
    return create(ContextConfig {});
}
}
