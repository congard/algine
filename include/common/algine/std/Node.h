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

    inline const std::string& getName() const;
    inline const std::vector<Node>& getChildren() const;
    inline const glm::mat4& getTransform() const;

private:
    std::string m_name;
    std::vector<Node> m_children;
    glm::mat4 m_defaultTransform {1.0f};
};

const std::string& Node::getName() const {
    return m_name;
}

const std::vector<Node>& Node::getChildren() const {
    return m_children;
}

const glm::mat4& Node::getTransform() const {
    return m_defaultTransform;
}
}

#endif /* ALGINE_NODE_H */
