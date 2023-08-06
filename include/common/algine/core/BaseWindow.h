#ifndef ALGINE_SURFACE_H
#define ALGINE_SURFACE_H

#include <algine/core/EventHandler.h>
#include <algine/core/Object.h>

#include <glm/vec2.hpp>

#include <forward_list>
#include <mutex>

namespace algine {
class Content;

class BaseWindow: public Object {
public:
    using Task = std::function<void()>;

public:
    explicit BaseWindow(Object *parent = defaultParent());
    ~BaseWindow() override;

    /**
     * Executes task `task` on the rendering thread.
     * @param task
     */
    void invokeLater(Task task);

    virtual void renderLoop() = 0;
    virtual void stopRenderLoop() = 0;

    virtual glm::ivec2 getViewport() = 0;

    /**
     * Sets the content and takes ownership of it.
     * @param content The content to set.
     * @param deleteOld If `true`, the old content will be deleted.
     * @note If `deleteOld` is set to `false`, then the old content
     * will still be a child of the window.
     */
    virtual void setContent(Content *content, bool deleteOld);

    /**
     * Sets the content, takes ownership and deletes the old content.
     * The same as `setContent(content, true)`.
     * @param content The content to set.
     */
    void setContent(Content *content);

    /**
     * Constructs content of type `T` on the rendering thread.
     * @tparam T The type of content to be constructed.
     * @tparam Args Types of the arguments.
     * @param args Arguments that will be passed to the content's constructor.
     * @param deleteOld If `true`, the old content will be deleted.
     */
    template<typename T, typename ...Args>
    void setContentLater(std::tuple<Args...> args, bool deleteOld = true) {
        invokeLater([args = std::move(args), this, deleteOld]() mutable {
            std::apply([this, deleteOld](auto&& ...args) {
                setContent(new T(std::forward<Args>(args)...), deleteOld);
            }, std::move(args));
        });
    }

    /**
     * Constructs content of type `T` on the rendering thread and deletes
     * the previous content object.
     * @tparam T The type of content to be constructed.
     * @tparam Args Types of the arguments.
     * @param args Arguments that will be passed to the content's constructor.
     */
    template<typename T, typename ...Args>
    void setContentLater(Args&& ...args) {
        setContentLater<T>(std::make_tuple(std::forward<Args>(args)...));
    }

    Content* getContent() const;

    void setEventHandler(EventHandler *eventHandler);
    EventHandler* getEventHandler() const;

protected:
    /**
     * Must be called from the rendering thread
     */
    void processTasks();

protected:
    Content *m_content {nullptr};
    EventHandler *m_eventHandler {nullptr};

private:
    std::forward_list<Task> m_taskQueue;
    std::forward_list<Task>::iterator m_taskQueueTail;
    std::mutex m_taskQueueMutex;
};
}

#endif //ALGINE_SURFACE_H
