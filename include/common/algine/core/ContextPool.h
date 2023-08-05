#ifndef ALGINE_CONTEXTPOOL_H
#define ALGINE_CONTEXTPOOL_H

#include <algine/core/Context.h>
#include <algine/core/ContextConfig.h>
#include <algine/types.h>

#include <mutex>

namespace algine {
class ContextPool {
public:
    constexpr static Index InvalidIndex = -1;

public:
    /**
     * Creates an empty ContextPool
     */
    ContextPool();

    ContextPool(int size, const ContextConfig &config);
    explicit ContextPool(int size);
    ~ContextPool();

    /**
     * Resizes the context pool to the specified size.
     * <ul>
     *   <li>If <code>size</code> is greater than the current size,
     *       new contexts will be added to the pool;</li>
     *   <li>If <code>size</code> is less than the current pool size,
     *       then the size will be decreased and extra contexts will
     *       be destroyed;</li>
     *   <li>Otherwise nothing will be changed.</li>
     * </ul>
     * @param size The new size of the pool
     * @param config
     */
    void resize(int size, const ContextConfig &config);

    /**
     * Resizes the context pool to the specified size.
     * @see <code>resize(int, const ContextConfig&)</code> for reference
     * @param size The new size of the pool
     */
    void resize(int size);

    /**
     * Attaches a context to the current thread.
     * @return The index of the attached context if there are available
     * contexts, <code>ContextPool::InvalidIndex</code> otherwise
     */
    Index attach();

    /**
     * Detaches the specified context from the current thread.
     * @param index The index of the attached context returned by ContextPool::attach
     * @return <code>true</code> if the detachment is successful, <code>false</code> otherwise
     */
    bool detach(Index index);

private:
    struct ContextDetails {
        Context context;
        bool isInUse {false};
    };

    std::vector<ContextDetails> m_pool;
    std::mutex m_mutex;
};
} // algine

#endif //ALGINE_CONTEXTPOOL_H
