#include <algine/std/model/Shape.h>
#include <algine/core/DataType.h>
#include <algine/core/TypeRegistry.h>

using namespace std;

namespace algine {
STATIC_INITIALIZER_IMPL(Shape) {
    alRegisterType(Shape);
}

Shape::Shape(Object *parent): Object(parent) {}

Shape::~Shape() = default;

void Shape::createInputLayout(const InputLayoutShapeLocations &locations) {
    auto inputLayout = new InputLayout(this);
    inputLayout->bind();
    m_inputLayouts.push_back(inputLayout);

    InputAttributeDescription attribDescription;
    attribDescription.setCount(3);

    auto addAttribute = [&](const ArrayBuffer *arrayBuffer) {
        if (attribDescription.m_location != InputAttributeDescription::LocationAbsent && arrayBuffer != nullptr) {
            inputLayout->addAttribute(attribDescription, arrayBuffer);
        }
    };

    attribDescription.setLocation(locations[InputLayoutShapeLocations::Position]);
    addAttribute(m_vertices);

    attribDescription.setLocation(locations[InputLayoutShapeLocations::Normal]);
    addAttribute(m_normals);

    attribDescription.setLocation(locations[InputLayoutShapeLocations::Tangent]);
    addAttribute(m_tangents);

    attribDescription.setLocation(locations[InputLayoutShapeLocations::Bitangent]);
    addAttribute(m_bitangents);

    attribDescription.setLocation(locations[InputLayoutShapeLocations::TexCoord]);
    attribDescription.setCount(2);
    addAttribute(m_texCoords);

    if (isBonesPresent()) {
        attribDescription.setCount(4);

        attribDescription.setLocation(locations[InputLayoutShapeLocations::BoneWeights]);
        addAttribute(m_boneWeights);

        attribDescription.setLocation(locations[InputLayoutShapeLocations::BoneIds]);
        attribDescription.setDataType(DataType::UnsignedInt);
        addAttribute(m_boneIds);
    }

    if (m_indices)
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

Index Shape::getAnimationIndexByName(string_view name) const {
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
}
