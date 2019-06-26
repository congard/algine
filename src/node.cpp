#include <algine/node.h>

namespace algine {
Node::Node(const aiNode *node) {
    name = node->mName.data;
    transformation = getMat4(node->mTransformation);
    childs.reserve(node->mNumChildren); // allocating memory
    for (size_t i = 0; i < node->mNumChildren; i++) childs.push_back(Node(node->mChildren[i]));
}

} /* namespace algine */