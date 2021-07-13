#include <algine/std/Node.h>

#include <assimp/scene.h>

#include "assimp2glm.h"

namespace algine {
Node::Node() = default;

Node::Node(const aiNode *node) {
    name = node->mName.data;
    defaultTransform = getMat4(node->mTransformation);

    children.reserve(node->mNumChildren); // allocating memory
    for (size_t i = 0; i < node->mNumChildren; i++)
        children.emplace_back(node->mChildren[i]);
}

Node* Node::getNode(const std::string &nodeName) {
    for (auto &child : children) {
        if (child.name == nodeName)
            return &child;
    }

    for (auto &child : children) {
        Node *node = child.getNode(nodeName);
        if (node != nullptr)
            return node;
    }

    return nullptr;
}

} /* namespace algine */
