#ifndef ALGINE_NODE_H
#define ALGINE_NODE_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include <algine/math.h>

namespace algine {
class Node {
public:
    std::string name;
    std::vector<Node> childs;
    glm::mat4 defaultTransform, transformation = glm::mat4(1.0f);

    Node();
    Node(const aiNode *node);

    Node* getNode(const std::string &name);
};

} /* namespace algine */

#endif /* ALGINE_NODE_H */
