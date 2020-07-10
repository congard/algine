#ifndef ALGINE_ANIMATOR_H
#define ALGINE_ANIMATOR_H

#include <algine/std/model/Shape.h>
#include <algine/types.h>

namespace algine {
class Animator {
public:
    Animator();
    Animator(Shape *shape, const std::string &animationName);
    explicit Animator(Shape *shape, usize animationIndex = 0);

    void animate(float timeInSeconds);

    void setShape(Shape *shape);
    void setAnimationIndex(usize animationIndex);
    void setAnimation(const std::string &name);

    Shape* getShape() const;
    usize getAnimationIndex() const;

private:
    void readNodeHierarchy(float animationTime, const Node &node, const glm::mat4 &parentTransform);

private:
    Shape *m_shape = nullptr;
    usize m_animationIndex = 0;
};
}

#endif //ALGINE_ANIMATOR_H
