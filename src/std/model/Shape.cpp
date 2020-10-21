#include <algine/std/model/Shape.h>

#include <algine/core/DataType.h>

#include "../../core/PublicObjectTools.h"

using namespace std;

namespace algine {
vector<ShapePtr> Shape::publicObjects;

Shape::~Shape() {
    for (auto &inputLayout : inputLayouts)
        InputLayout::destroy(inputLayout);

    ArrayBuffer::destroy(vertices, normals, texCoords, tangents, bitangents, boneWeights, boneIds);
    IndexBuffer::destroy(indices);
}

void Shape::createInputLayout(const InputLayoutShapeLocations &locations) {
    auto inputLayout = new InputLayout();
    inputLayout->bind();
    inputLayouts.push_back(inputLayout);

    InputAttributeDescription attribDescription;
    attribDescription.setCount(3);

    auto addAttribute = [&](const ArrayBuffer *arrayBuffer)
    {
        if (attribDescription.m_location != InputAttributeDescription::LocationAbsent && arrayBuffer != nullptr) {
            inputLayout->addAttribute(attribDescription, arrayBuffer);
        }
    };

    attribDescription.setLocation(locations.position);
    addAttribute(vertices);

    attribDescription.setLocation(locations.normal);
    addAttribute(normals);

    attribDescription.setLocation(locations.tangent);
    addAttribute(tangents);

    attribDescription.setLocation(locations.bitangent);
    addAttribute(bitangents);

    attribDescription.setLocation(locations.texCoord);
    attribDescription.setCount(2);
    addAttribute(texCoords);

    if (isBonesPresent()) {
        attribDescription.setCount(4);

        attribDescription.setLocation(locations.boneWeights);
        addAttribute(boneWeights);

        attribDescription.setLocation(locations.boneIds);
        attribDescription.setDataType(DataType::UnsignedInt);
        addAttribute(boneIds);
    }

    inputLayout->setIndexBuffer(indices);
    inputLayout->unbind();
}

void Shape::setBoneTransform(const string &boneName, const glm::mat4 &transformation) {
    if (uint index = bonesStorage.getIndex(boneName); index != BonesStorage::BoneNotFound) {
        bonesStorage.bones[index].transformation = transformation;
    } else {
        throw runtime_error("Bone " + boneName + " does not found");
    }
}

void Shape::prepareAnimation(uint index) {
    animations[index].bones.resize(bonesStorage.count());
}

void Shape::invalidateAnimation(uint index) {
    animations[index].bones.clear();
}

bool Shape::isAnimationValid(uint index) const {
    return animations[index].bones.size() == bonesStorage.count();
}

bool Shape::isBonesPresent() const {
    return bonesPerVertex > 0;
}

usize Shape::getAnimationIndexByName(const string &name) {
    for (usize i = 0; i < animations.size(); i++)
        if (animations[i].name == name)
            return i;

    return AnimationNotFound;
}

ShapePtr Shape::getByName(const string &name) {
    return PublicObjectTools::getByName<ShapePtr>(name);
}

Shape* Shape::byName(const string &name) {
    return PublicObjectTools::byName<Shape>(name);
}
}
