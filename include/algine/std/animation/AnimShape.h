#ifndef ALGINE_ANIMSHAPE_H
#define ALGINE_ANIMSHAPE_H

#include <algine/std/animation/Animation.h>
#include <algine/std/animation/Bone.h>
#include <algine/std/Node.h>

namespace algine {
class AnimShape {
public:
    AnimShape();
    AnimShape(std::vector<Animation> *animations, std::vector<Bone> *bones, glm::mat4 *globalInverseTransform, Node *rootNode);

public:
    std::vector<Animation> *animations = nullptr;
    std::vector<Bone> *bones = nullptr;
    glm::mat4 *globalInverseTransform = nullptr;
    Node *rootNode = nullptr;
};
}

#endif //ALGINE_ANIMSHAPE_H
