#ifndef ALGINE_ANIMATOR_H
#define ALGINE_ANIMATOR_H

#include <algine/std/model/Shape.h>
#include <algine/types.h>

namespace algine {
class Animator {
public:
    Animator();
    Animator(const ShapePtr &shape, const std::string &animationName);
    explicit Animator(const ShapePtr &shape, usize animationIndex = 0);

    void animate(float timeInSeconds);

    void setShape(const ShapePtr &shape);
    void setAnimationIndex(usize animationIndex);
    void setAnimation(const std::string &name);

    const ShapePtr& getShape() const;
    usize getAnimationIndex() const;

private:
    void readNodeHierarchy(float animationTime, const Node &node, const glm::mat4 &parentTransform);

private:
    ShapePtr m_shape = nullptr;
    usize m_animationIndex = 0;
};
}

#endif //ALGINE_ANIMATOR_H
