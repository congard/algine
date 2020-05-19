#include <algine/std/node.h>

#include <assimp/scene.h>

namespace algine {
Node::Node() = default;

Node::Node(const aiNode *node) {
    name = node->mName.data;
    defaultTransform = getMat4(node->mTransformation);

    childs.reserve(node->mNumChildren); // allocating memory
    for (size_t i = 0; i < node->mNumChildren; i++)
        childs.emplace_back(node->mChildren[i]);
}

Node* Node::getNode(const std::string &nodeName) {
    for (auto &child : childs) {
        if (child.name == nodeName)
            return &child;
    }

    for (auto &child : childs) {
        Node *node = child.getNode(nodeName);
        if (node != nullptr)
            return node;
    }

    return nullptr;
}

} /* namespace algine */
