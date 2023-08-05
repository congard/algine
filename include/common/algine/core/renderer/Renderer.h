#ifndef ALGINE_SCENERENDERER_H
#define ALGINE_SCENERENDERER_H

#include <algine/core/scene/GlobalScene.h>
#include <algine/core/Object.h>
#include <algine/types.h>

namespace algine {
class RenderPass;

class Renderer: public Object {
public:
    explicit Renderer(Scene *parent = GlobalScene::getInstance());

    /**
     * The same as setParent
     * @param parent
     */
    void setScene(Scene *parent);

    /**
     * @param parent must be a subclass of Scene
     */
    void setParent(Object *parent) override;

    void addPass(RenderPass *pass);
    void insertPass(RenderPass *pass, Index index);

    /**
     * Removes render pass from the list.
     * @param pass the pass to remove
     * @param autoDelete if `true`, then in case of
     * success `pass` will be deleted
     * @return `true` in case of success, `false` otherwise
     */
    bool removePass(RenderPass *pass, bool autoDelete = true);

    const std::vector<RenderPass*>& getPasses() const;

    void render();

private:
    std::vector<RenderPass*> m_passes;
};
} // algine

#endif //ALGINE_SCENERENDERER_H
