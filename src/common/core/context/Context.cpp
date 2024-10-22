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

Context::DestroySubject::Subscription_t
Context::addOnDestroyListener(DestroySubject::ObserverAutoPtr_t observer) {
    return getOnDestroy().subscribe(std::move(observer));
}

Context::DestroySubject& Context::getOnDestroy() {
    // https://isocpp.org/wiki/faq/ctors#static-init-order
    static DestroySubject onDestroy;
    return onDestroy;
}
}
