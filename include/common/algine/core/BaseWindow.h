#ifndef ALGINE_SURFACE_H
#define ALGINE_SURFACE_H

#include <algine/core/EventHandler.h>
#include <algine/core/Ptr.h>

#include <glm/vec2.hpp>

namespace algine {
class Content;

class BaseWindow {
public:
    virtual ~BaseWindow();

    virtual void renderLoop() = 0;
    virtual void stopRenderLoop() = 0;

    virtual glm::ivec2 getViewport() = 0;

    virtual void setContent(const Ptr<Content> &content);
    void setContent(Content *content);
    const Ptr<Content>& getContent() const;

    void setEventHandler(EventHandler *eventHandler);
    EventHandler* getEventHandler() const;

protected:
    Ptr<Content> m_content {nullptr};
    EventHandler *m_eventHandler {nullptr};
};
}

#endif //ALGINE_SURFACE_H
