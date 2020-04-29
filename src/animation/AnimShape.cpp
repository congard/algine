#include <algine/animation/AnimShape.h>

using namespace std;

namespace algine {
AnimShape::AnimShape() = default;

AnimShape::AnimShape(vector<Animation> *animations, vector<Bone> *bones, glm::mat4 *globalInverseTransform, Node *rootNode) {
    this->animations = animations;
    this->bones = bones;
    this->globalInverseTransform = globalInverseTransform;
    this->rootNode = rootNode;
}
}