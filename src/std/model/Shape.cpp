#include <algine/std/model/Shape.h>

namespace algine {
inline void
addAttribute(
        const InputLayout *inputLayout,
        const InputAttributeDescription &inputAttributeDescription,
        const ArrayBuffer *arrayBuffer)
{
    if (inputAttributeDescription.m_location != InputAttributeDescription::LocationAbsent && arrayBuffer != nullptr)
        inputLayout->addAttribute(inputAttributeDescription, arrayBuffer);
}

void Shape::createInputLayout(
        int inPosition, int inTexCoord, int inNormal,
        int inTangent, int inBitangent,
        int inBoneWeights, int inBoneIds
) {
    auto *inputLayout = new InputLayout();
    inputLayout->bind();
    inputLayouts.push_back(inputLayout);

    InputAttributeDescription attribDescription;
    attribDescription.setCount(3);

    attribDescription.setLocation(inPosition);
    addAttribute(inputLayout, attribDescription, buffers.vertices);

    attribDescription.setLocation(inNormal);
    addAttribute(inputLayout, attribDescription, buffers.normals);

    attribDescription.setLocation(inTangent);
    addAttribute(inputLayout, attribDescription, buffers.tangents);

    attribDescription.setLocation(inBitangent);
    addAttribute(inputLayout, attribDescription, buffers.bitangents);

    attribDescription.setLocation(inTexCoord);
    attribDescription.setCount(2);
    addAttribute(inputLayout, attribDescription, buffers.texCoords);

    if (bonesPerVertex != 0) {
        attribDescription.setCount(4);

        attribDescription.setLocation(inBoneWeights);
        addAttribute(inputLayout, attribDescription, buffers.boneWeights);

        attribDescription.setLocation(inBoneIds);
        attribDescription.setFormat(GL_UNSIGNED_INT); // TODO
        addAttribute(inputLayout, attribDescription, buffers.boneIds);
    }

    inputLayout->setIndexBuffer(buffers.indices);
    inputLayout->unbind();
}

void Shape::setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation) {
    rootNode.getNode(nodeName)->transformation = transformation;
}

void Shape::recycle() {
    for (auto &item : inputLayouts)
        InputLayout::destroy(item);

    ArrayBuffer::destroy(buffers.vertices, buffers.normals, buffers.texCoords,
                         buffers.tangents, buffers.bitangents, buffers.boneWeights, buffers.boneIds);
    IndexBuffer::destroy(buffers.indices);
}

void Shape::prepareAnimation(const uint index) {
    animations[index].bones.resize(bonesStorage.count());
}

void Shape::invalidateAnimation(const uint index) {
    animations[index].bones.clear();
}

bool Shape::isAnimationValid(const uint index) {
    return animations[index].bones.size() == bonesStorage.count();
}

usize Shape::getAnimationIndexByName(const std::string &name) {
    for (usize i = 0; i < animations.size(); i++)
        if (animations[i].name == name)
            return i;

    return AnimationNotFound;
}
}
