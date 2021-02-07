#ifndef ALGINE_NODE_H
#define ALGINE_NODE_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>

class aiNode;

namespace algine {
class Node {
public:
    Node();
    explicit Node(const aiNode *node);

    Node* getNode(const std::string &nodeName);

public:
    std::string name;
    std::vector<Node> childs;
    glm::mat4 defaultTransform = glm::mat4(1.0f);
};

} /* namespace algine */

#endif /* ALGINE_NODE_H */
