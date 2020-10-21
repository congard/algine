#ifndef ALGINE_SHAPE_H
#define ALGINE_SHAPE_H

#include <algine/std/Node.h>
#include <algine/std/animation/Animation.h>
#include <algine/std/animation/BonesStorage.h>
#include <algine/std/model/Mesh.h>
#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/std/model/ShapePtr.h>

#include <algine/core/InputLayout.h>
#include <algine/core/Object.h>

namespace algine {
class Shape: public Object {
public:
    constexpr static usize AnimationNotFound = -1;

public:
    ~Shape();

    /// creates InputLayout and adds it into inputLayouts array
    /// <br>note: this function will limit max bones per vertex to 4
    /// <br>if you need more, you will have to create InputLayout manually
    void createInputLayout(const InputLayoutShapeLocations &locations);

    void setBoneTransform(const std::string &boneName, const glm::mat4 &transformation);

    void prepareAnimation(uint index);
    void invalidateAnimation(uint index);

    bool isAnimationValid(uint index) const;
    bool isBonesPresent() const;

    usize getAnimationIndexByName(const std::string &name);

public:
    static ShapePtr getByName(const std::string &name);
    static Shape* byName(const std::string &name);

public:
    static std::vector<ShapePtr> publicObjects;

public:
    std::vector<Mesh> meshes;
    std::vector<Animation> animations;
    glm::mat4 globalInverseTransform;
    std::vector<InputLayout*> inputLayouts;
    BonesStorage bonesStorage;
    Node rootNode;
    uint bonesPerVertex;

public:
    ArrayBuffer *vertices, *normals, *texCoords, *tangents, *bitangents, *boneWeights, *boneIds;
    IndexBuffer *indices;
};
}

#endif //ALGINE_SHAPE_H
