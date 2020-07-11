#ifndef ALGINE_SHAPE_H
#define ALGINE_SHAPE_H

#include <algine/std/Node.h>
#include <algine/std/animation/Animation.h>
#include <algine/std/animation/BonesStorage.h>
#include <algine/std/model/Mesh.h>
#include <algine/core/InputLayout.h>

namespace algine {
class Shape {
public:
    constexpr static usize AnimationNotFound = -1;

public:
    // TODO: move it to ShapeLoader
    // creates InputLayout and adds it into inputLayouts array
    // note: this function will limit max bones per vertex to 4
    // if you need more, you will have to create InputLayout manually
    void createInputLayout(
            int inPosition, int inTexCoord = -1, int inNormal = -1,
            int inTangent = -1, int inBitangent = -1,
            int inBoneWeights = -1, int inBoneIds = -1
    );

    void setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation);
    void recycle();

    usize getAnimationIndexByName(const std::string &name);

public:
    std::vector<Mesh> meshes;
    std::vector<Animation> animations;
    glm::mat4 globalInverseTransform;
    std::vector<InputLayout*> inputLayouts;
    BonesStorage bonesStorage;
    Node rootNode;
    uint bonesPerVertex = 0;

public:
    struct Geometry {
        std::vector<float> vertices, normals, texCoords, tangents, bitangents, boneWeights;
        std::vector<uint> indices, boneIds;
    } geometry;

    struct Buffers {
        ArrayBuffer *vertices, *normals, *texCoords, *tangents, *bitangents, *boneWeights, *boneIds;
        IndexBuffer *indices;
    } buffers;
};
}

#endif //ALGINE_SHAPE_H
