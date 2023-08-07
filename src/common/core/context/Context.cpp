#include <algine/core/context/Context.h>
#include <algine/core/context/ContextConfig.h>
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

tulz::Subscription<Context*> Context::addOnDestroyListener(const tulz::Observer<Context*> &observer) {
    return getOnDestroy().subscribe(observer);
}

tulz::Subject<Context*>& Context::getOnDestroy() {
    // https://isocpp.org/wiki/faq/ctors#static-init-order
    static tulz::Subject<Context*> onDestroy;
    return onDestroy;
}
}
