#include <algine/std/model/Shape.h>

#include <algine/core/DataType.h>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;

namespace algine {
vector<ShapePtr> Shape::publicObjects;

Shape::~Shape() {
    for (auto &inputLayout : m_inputLayouts)
        delete inputLayout;

    for (auto &p : {m_vertices, m_normals, m_texCoords, m_tangents, m_bitangents, m_boneWeights, m_boneIds}) {
        delete p;
    }

    delete m_indices;
}

void Shape::createInputLayout(const InputLayoutShapeLocations &locations) {
    auto inputLayout = new InputLayout();
    inputLayout->bind();
    m_inputLayouts.push_back(inputLayout);

    InputAttributeDescription attribDescription;
    attribDescription.setCount(3);

    auto addAttribute = [&](const ArrayBuffer *arrayBuffer)
    {
        if (attribDescription.m_location != InputAttributeDescription::LocationAbsent && arrayBuffer != nullptr) {
            inputLayout->addAttribute(attribDescription, arrayBuffer);
        }
    };

    attribDescription.setLocation(locations.position);
    addAttribute(m_vertices);

    attribDescription.setLocation(locations.normal);
    addAttribute(m_normals);

    attribDescription.setLocation(locations.tangent);
    addAttribute(m_tangents);

    attribDescription.setLocation(locations.bitangent);
    addAttribute(m_bitangents);

    attribDescription.setLocation(locations.texCoord);
    attribDescription.setCount(2);
    addAttribute(m_texCoords);

    if (isBonesPresent()) {
        attribDescription.setCount(4);

        attribDescription.setLocation(locations.boneWeights);
        addAttribute(m_boneWeights);

        attribDescription.setLocation(locations.boneIds);
        attribDescription.setDataType(DataType::UnsignedInt);
        addAttribute(m_boneIds);
    }

    inputLayout->setIndexBuffer(m_indices);
    inputLayout->unbind();
}

void Shape::setMeshes(const vector<Mesh> &meshes) {
    m_meshes = meshes;
}

void Shape::setAnimations(const vector<Animation> &animations) {
    m_animations = animations;
}

void Shape::addMesh(const Mesh &mesh) {
    m_meshes.emplace_back(mesh);
}

void Shape::addAnimation(const Animation &animation) {
    m_animations.emplace_back(animation);
}

bool Shape::isBonesPresent() const {
    return m_bonesPerVertex > 0;
}

bool Shape::isAnimationsPresent() const {
    return !m_animations.empty();
}

const std::vector<Mesh>& Shape::getMeshes() const {
    return m_meshes;
}

const std::vector<Animation>& Shape::getAnimations() const {
    return m_animations;
}

const std::vector<InputLayout*>& Shape::getInputLayouts() const {
    return m_inputLayouts;
}

const glm::mat4& Shape::getGlobalInverseTransform() const {
    return m_globalInverseTransform;
}

const BonesStorage& Shape::getBones() const {
    return m_bones;
}

const Node& Shape::getRootNode() const {
    return m_rootNode;
}

uint Shape::getBonesPerVertex() const {
    return m_bonesPerVertex;
}

const Animation& Shape::getAnimation(Index index) const {
    return m_animations[index];
}

Index Shape::getAnimationIndexByName(const string &name) const {
    for (Index i = 0; i < m_animations.size(); i++) {
        if (m_animations[i].getName() == name) {
            return i;
        }
    }

    return AnimationNotFound;
}

uint Shape::getAnimationsAmount() const {
    return m_animations.size();
}

uint Shape::getBonesAmount() const {
    return m_bones.size();
}

uint Shape::getInputLayoutsAmount() const {
    return m_inputLayouts.size();
}

InputLayout* Shape::getInputLayout(Index index) {
    return m_inputLayouts[index];
}

RawPtr<ArrayBuffer> Shape::getVerticesBuffer() const {
    return m_vertices;
}

RawPtr<ArrayBuffer> Shape::getNormalsBuffer() const {
    return m_normals;
}

RawPtr<ArrayBuffer> Shape::getTexCoordsBuffer() const {
    return m_texCoords;
}

RawPtr<ArrayBuffer> Shape::getTangentsBuffer() const {
    return m_tangents;
}

RawPtr<ArrayBuffer> Shape::getBitangentsBuffer() const {
    return m_bitangents;
}

RawPtr<ArrayBuffer> Shape::getBoneWeightsBuffer() const {
    return m_boneWeights;
}

RawPtr<ArrayBuffer> Shape::getBoneIdsBuffer() const {
    return m_boneIds;
}

RawPtr<IndexBuffer> Shape::getIndicesBuffer() const {
    return m_indices;
}

ShapePtr Shape::getByName(const string &name) {
    return PublicObjectTools::getByName<ShapePtr>(name);
}

Shape* Shape::byName(const string &name) {
    return PublicObjectTools::byName<Shape>(name);
}
}
