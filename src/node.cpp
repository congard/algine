#ifndef ALGINE_NODE_CPP
#define ALGINE_NODE_CPP

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include "math.cpp"

namespace algine {
struct Node {
    std::string name;
    std::vector<Node> childs;
    glm::mat4 transformation;

    Node() { /* empty */ }

    Node(const aiNode *node) {
        name = node->mName.data;
        transformation = getMat4(node->mTransformation);
        childs.reserve(node->mNumChildren); // allocating memory
        for (size_t i = 0; i < node->mNumChildren; i++) childs.push_back(Node(node->mChildren[i]));
    }
};

} /* namespace algine */

#endif /* ALGINE_NODE_CPP */