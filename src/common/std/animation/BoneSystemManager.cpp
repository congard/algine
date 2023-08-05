#include <algine/std/animation/BoneSystemManager.h>

#include <algine/std/model/Model.h>
#include <algine/std/model/Shape.h>

#include <algine/std/animation/BoneSystemModuleConstants.h>

#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

namespace algine {
constexpr uint empty_block = 0;

void BoneSystemManager::init(Object *bufferParent) {
    using namespace BoneSystemModuleConstants::Vars::Block;

    m_uniformBlock.setName(Name);
    m_uniformBlock.setVarNames({Bones, BoneAttribsPerVertex});

    for (const auto &program : m_programs)
        m_uniformBlock.assignBindingPoint(program);

    m_uniformBlock.init(m_programs[0]);

    m_bonesPos = m_uniformBlock.getVarPosition(Bones);
    m_boneAttribsCountPos = m_uniformBlock.getVarPosition(BoneAttribsPerVertex);

    m_bufferStorage.setBufferType(Buffer::Uniform);
    m_bufferStorage.setBufferUsage(Buffer::DynamicDraw);
    m_bufferStorage.setBlockSize(m_uniformBlock.getSize(), UniformBuffer::getOffsetAlignment());
    m_bufferStorage.allocateStorage(bufferParent);

    int zero = 0;

    m_bufferStorage.allocateBlock(empty_block); // for default (empty) block
    m_bufferStorage.write(
            empty_block, m_uniformBlock.getVarOffset(m_boneAttribsCountPos), sizeof(int), &zero);

    m_uniformBlock.setBuffer(reinterpret_cast<UniformBuffer*>(m_bufferStorage.getBuffer()));
}

void BoneSystemManager::writeBonesForAll() {
    for (const auto &p : m_ids) {
        writeBones(p.first, p.second);
    }
}

void BoneSystemManager::writeBones(Model *model) {
    if (model->getShape()->isBonesPresent()) {
        writeBones(model, m_ids[model]);
    }
}

void BoneSystemManager::linkBuffer(Model *model) {
    if (model->getShape()->isBonesPresent()) {
        linkUniformBuffer(m_ids[model]);
    } else {
        linkUniformBuffer(empty_block);
    }
}

void BoneSystemManager::setupBones(Model *model) {
    if (model->getShape()->isBonesPresent()) {
        uint index = m_ids[model];
        writeBones(model, index);
        linkUniformBuffer(index);
    } else {
        linkUniformBuffer(empty_block);
    }
}

void BoneSystemManager::setMaxModelsCount(uint count) {
    m_bufferStorage.setBlocksCount(count + 1);
}

void BoneSystemManager::setBindingPoint(uint bindingPoint) {
    m_uniformBlock.setBindingPoint(bindingPoint);
}

void BoneSystemManager::setShaderPrograms(const vector<ShaderProgram*> &shaderPrograms) {
    m_programs = shaderPrograms;
}

void BoneSystemManager::addShaderProgram(ShaderProgram *shaderProgram) {
    m_programs.emplace_back(shaderProgram);
}

void BoneSystemManager::addModel(Model *model) {
    uint index = m_bufferStorage.allocateBlock();
    m_ids[model] = index;

    int attribsCount = getAttribsCount(model->getShape()->getBonesPerVertex());
    m_bufferStorage.write(index, m_uniformBlock.getVarOffset(m_boneAttribsCountPos), sizeof(int), &attribsCount);
}

void BoneSystemManager::addModels(const std::vector<Model*> &models) {
    for (const auto &model : models) {
        addModel(model);
    }
}

void BoneSystemManager::removeModel(Model *model) {
    m_bufferStorage.freeBlock(m_ids[model]);
    m_ids.erase(model);
}

void BoneSystemManager::removeModels(const std::vector<Model*> &models) {
    for (const auto &model : models) {
        removeModel(model);
    }
}

uint BoneSystemManager::getBindingPoint() const {
    return m_uniformBlock.getBindingPoint();
}

const vector<ShaderProgram*>& BoneSystemManager::getShaderPrograms() const {
    return m_programs;
}

const UniformBlock& BoneSystemManager::getUniformBlock() const {
    return m_uniformBlock;
}

const BlockBufferStorage& BoneSystemManager::getBlockBufferStorage() const {
    return m_bufferStorage;
}

uint BoneSystemManager::getAttribsCount(const uint bonesPerVertex) {
    return bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1);
}

void BoneSystemManager::writeBones(Model *model, Index index) {
    const auto &bones = *(model->getBones());

    m_bufferStorage.write(
        index, m_uniformBlock.getVarOffset(m_bonesPos),
        sizeof(mat4) * bones.size(), value_ptr(bones[0])
    );
}

void BoneSystemManager::linkUniformBuffer(Index blockIndex) {
    if (blockIndex != m_linkedBlock) {
        m_linkedBlock = blockIndex;
        m_uniformBlock.linkBuffer(m_bufferStorage.getBlockSize() * blockIndex, m_uniformBlock.getSize());
    }
}
}
