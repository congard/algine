#ifndef ALGINE_NODE_H
#define ALGINE_NODE_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include <algine/math.h>

namespace algine {
struct Node {
    std::string name;
    std::vector<Node> childs;
    glm::mat4 transformation;

    Node() { /* empty */ }

    Node(const aiNode *node);
};

} /* namespace algine */

#endif /* ALGINE_NODE_H */