#include <algine/node.h>
#include <algine/types.h>

namespace algine {
Node::Node() = default;

Node::Node(const aiNode *node) {
    name = node->mName.data;
    defaultTransform = getMat4(node->mTransformation);
    childs.reserve(node->mNumChildren); // allocating memory
    for (size_t i = 0; i < node->mNumChildren; i++) childs.push_back(Node(node->mChildren[i]));
}

Node* Node::getNode(const std::string &name) {
    for (usize i = 0; i < childs.size(); i++) {
        if (childs[i].name == name)
            return &childs[i];
    }

    Node *node;
    for (usize i = 0; i < childs.size(); i++) {
        node = childs[i].getNode(name);
        if (node != nullptr)
            return node;
    }

    return nullptr;
}

} /* namespace algine */
