#ifndef ALGINE_SHAPE_H
#define ALGINE_SHAPE_H

#include <algine/std/Node.h>
#include <algine/std/animation/Animation.h>
#include <algine/std/animation/BonesStorage.h>
#include <algine/std/model/Mesh.h>
#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/core/InputLayout.h>

namespace algine {
class Shape {
public:
    constexpr static usize AnimationNotFound = -1;

public:
    void setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation);
    void recycle();

    /// creates InputLayout and adds it into inputLayouts array
    /// <br>note: this function will limit max bones per vertex to 4
    /// <br>if you need more, you will have to create InputLayout manually
    void createInputLayout(const InputLayoutShapeLocations &locations);

    void prepareAnimation(uint index);
    void invalidateAnimation(uint index);
    bool isAnimationValid(uint index);

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
