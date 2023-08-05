#ifndef ALGINE_RENDERPASS_H
#define ALGINE_RENDERPASS_H

#include <algine/core/Object.h>

namespace algine {
class Renderer;

class RenderPass: public Object {
public:
    explicit RenderPass(Renderer *parent);

    void setParent(Object *parent) override;

    virtual void render() = 0;
};
} // algine

#endif //ALGINE_RENDERPASS_H
