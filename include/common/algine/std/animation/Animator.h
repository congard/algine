#ifndef ALGINE_ANIMATOR_H
#define ALGINE_ANIMATOR_H

#include <algine/std/Node.h>
#include <algine/types.h>

namespace algine {
class Model;

class Animator {
public:
    Animator();
    Animator(Model *model, const std::string &animationName);
    explicit Animator(Model *model, Index animationIndex = 0);

    void animate(float timeInSeconds);

    void setModel(Model *model);
    void setAnimationIndex(Index animationIndex);
    void setAnimation(const std::string &name);

    Model* getModel() const;
    Index getAnimationIndex() const;

private:
    void readNodeHierarchy(float animationTime, const Node &node, const glm::mat4 &parentTransform);

private:
    Model *m_model = nullptr;
    Index m_animationIndex = 0;
};
}

#endif //ALGINE_ANIMATOR_H
