#include <algine/std/Node.h>

#include <assimp/scene.h>

#include "assimp2glm.h"

namespace algine {
Node::Node() = default;

Node::Node(const aiNode *node)
    : m_name(node->mName.data),
      m_defaultTransform(getMat4(node->mTransformation))
{
    m_children.reserve(node->mNumChildren); // allocating memory

    for (size_t i = 0; i < node->mNumChildren; i++) {
        m_children.emplace_back(node->mChildren[i]);
    }
}

Node* Node::getNode(const std::string &nodeName) {
    for (auto &child : m_children) {
        if (child.getName() == nodeName) {
            return &child;
        }
    }

    for (auto &child : m_children) {
        if (Node *node = child.getNode(nodeName); node != nullptr) {
            return node;
        }
    }

    return nullptr;
}
}
