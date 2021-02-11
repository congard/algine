#ifndef ALGINE_SURFACE_H
#define ALGINE_SURFACE_H

#include <algine/core/EventHandler.h>
#include <algine/core/Ptr.h>

#include <glm/vec2.hpp>

namespace algine {
class Content;

class Surface {
public:
    virtual void renderLoop() = 0;

    virtual void stopRenderLoop() = 0;

    virtual const glm::ivec2& getViewport();

    virtual void setContent(const Ptr<Content> &content);
    void setContent(Content *content);

    void setEventHandler(EventHandler *eventHandler);

    const Ptr<Content>& getContent() const;

    EventHandler* getEventHandler() const;

protected:
    void initSurfaceFields();

protected:
    Ptr<Content> m_content;
    EventHandler *m_eventHandler;
    glm::ivec2 m_viewport;
};
}

#endif //ALGINE_SURFACE_H
