#ifndef ALGINE_SHAPE_H
#define ALGINE_SHAPE_H

#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/std/model/Mesh.h>
#include <algine/std/model/ShapePtr.h>

#include <algine/std/animation/Animation.h>
#include <algine/std/animation/BonesStorage.h>
#include <algine/std/Node.h>

#include <algine/core/InputLayout.h>
#include <algine/core/Object.h>
#include <algine/core/RawPtr.h>

namespace algine {
class Shape: public Object {
    friend class ShapeBuilder;
    friend class Model;
    friend class Animator;
    friend class AnimationBlender;

    STATIC_INITIALIZER_DECL

public:
    constexpr static Index AnimationNotFound = -1;

public:
    virtual ~Shape();

    /// creates InputLayout and adds it into inputLayouts data
    /// <br>note: this function will limit max bones per vertex to 4
    /// <br>if you need more, you will have to create InputLayout manually
    void createInputLayout(const InputLayoutShapeLocations &locations);

    void setMeshes(const std::vector<Mesh> &meshes);
    void setAnimations(const std::vector<Animation> &animations);
    void addMesh(const Mesh &mesh);
    void addAnimation(const Animation &animation);

    bool isBonesPresent() const;
    bool isAnimationsPresent() const;

    const std::vector<Mesh>& getMeshes() const;
    const std::vector<Animation>& getAnimations() const;
    const std::vector<InputLayout*>& getInputLayouts() const;
    const glm::mat4& getGlobalInverseTransform() const;
    const BonesStorage& getBones() const;
    const Node& getRootNode() const;
    uint getBonesPerVertex() const;

    const Animation& getAnimation(Index index) const;
    Index getAnimationIndexByName(std::string_view name) const;
    uint getAnimationsAmount() const;
    uint getBonesAmount() const;
    uint getInputLayoutsAmount() const;
    InputLayout* getInputLayout(Index index);

    RawPtr<ArrayBuffer> getVerticesBuffer() const;
    RawPtr<ArrayBuffer> getNormalsBuffer() const;
    RawPtr<ArrayBuffer> getTexCoordsBuffer() const;
    RawPtr<ArrayBuffer> getTangentsBuffer() const;
    RawPtr<ArrayBuffer> getBitangentsBuffer() const;
    RawPtr<ArrayBuffer> getBoneWeightsBuffer() const;
    RawPtr<ArrayBuffer> getBoneIdsBuffer() const;

    RawPtr<IndexBuffer> getIndicesBuffer() const;

public:
    static ShapePtr getByName(const std::string &name);
    static Shape* byName(const std::string &name);

public:
    static std::vector<ShapePtr> publicObjects;

protected:
    std::vector<Mesh> m_meshes;
    std::vector<Animation> m_animations;
    std::vector<InputLayout*> m_inputLayouts;
    glm::mat4 m_globalInverseTransform;
    BonesStorage m_bones;
    Node m_rootNode;
    uint m_bonesPerVertex;

protected:
    RawPtr<ArrayBuffer> m_vertices, m_normals, m_texCoords;
    RawPtr<ArrayBuffer> m_tangents, m_bitangents, m_boneWeights, m_boneIds;
    RawPtr<IndexBuffer> m_indices;
};
}

#endif //ALGINE_SHAPE_H
