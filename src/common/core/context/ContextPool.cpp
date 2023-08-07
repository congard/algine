#include <algine/core/context/ContextPool.h>
#include <algine/core/Engine.h>

namespace algine {
ContextPool::ContextPool(): ContextPool(0) {}

ContextPool::ContextPool(int size, const ContextConfig &config) {
    m_pool.reserve(size);

    for (int i = 0; i < size; ++i) {
        auto &details = m_pool.emplace_back();
        details.context.create(config);
    }
}

ContextPool::ContextPool(int size)
    : ContextPool(size, {.parent = Engine::getApplicationContext()}) {}

ContextPool::~ContextPool() {
    for (auto &details : m_pool) {
        details.context.destroy();
    }
}

void ContextPool::resize(int size, const ContextConfig &config) {
    if (size == m_pool.size())
        return;

    auto oldSize = static_cast<int>(m_pool.size());

    if (size > m_pool.size()) {
        m_pool.resize(size);

        for (int i = oldSize; i < size; ++i) {
            m_pool[i].context.create(config);
        }
    } else {
        for (int i = size; i < oldSize; ++i)
            m_pool[i].context.destroy();
        m_pool.resize(size);
    }
}

void ContextPool::resize(int size) {
    resize(size, {.parent = Engine::getApplicationContext()});
}

Index ContextPool::attach() {
    std::lock_guard lockGuard(m_mutex);

    for (uint i = 0; i < m_pool.size(); ++i) {
        auto &details = m_pool[i];

        if (!details.isInUse) {
            details.isInUse = true;
            details.context.makeCurrent();
            return i;
        }
    }

    return InvalidIndex;
}

bool ContextPool::detach(Index index) {
    if (index >= m_pool.size())
        return false;

    std::lock_guard lockGuard(m_mutex);

    if (auto &details = m_pool[index]; !details.isInUse) {
        return false;
    } else {
        if (details.context.detach())
            details.isInUse = false;
        return !details.isInUse;
    }
}
} // algine