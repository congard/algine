#include <algine/std/animation/BoneSystemManager.h>

#include <algine/constants/BoneSystem.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;
using namespace glm;

namespace algine {
BoneSystemManager::~BoneSystemManager() {
    for (auto & p : m_buffers)
        UniformBuffer::destroy(p.second);

    UniformBuffer::destroy(m_emptyBuffer);
}

void BoneSystemManager::init() {
    using namespace Module::BoneSystem::Vars::Block;

    m_uniformBlock.setName(Name);
    m_uniformBlock.setVarNames({Bones, BoneAttribsPerVertex});

    for (auto program : m_programs)
        m_uniformBlock.assignBindingPoint(program);

    m_uniformBlock.loadBlockInfo(m_programs[0]);

    m_bonesPos = m_uniformBlock.getVarPosition(Bones);
    m_boneAttribsCountPos = m_uniformBlock.getVarPosition(BoneAttribsPerVertex);

    m_emptyBuffer = new UniformBuffer();
    m_uniformBlock.setBuffer(m_emptyBuffer);
    m_uniformBlock.configureBuffer();

    m_emptyBuffer->bind();
    m_uniformBlock.writeInt(m_boneAttribsCountPos, 0);
    m_emptyBuffer->unbind();
}

void BoneSystemManager::initBuffer(Model *model) {
    if (model->getShape()->isBonesPresent()) {
        auto buffer = new UniformBuffer();
        m_uniformBlock.setBuffer(buffer);
        m_uniformBlock.configureBuffer();

        buffer->bind();
        m_uniformBlock.writeInt(m_boneAttribsCountPos, getAttribsCount(model->getShape()->bonesPerVertex));
        buffer->unbind();

        m_buffers[model] = buffer;
    }
}

void BoneSystemManager::initBuffers(const std::vector<Model*> &models) {
    for (auto model : models)
        initBuffer(model);
}

inline void linkUniformBuffer(UniformBlock &uniformBlock, UniformBuffer *buffer) {
    if (uniformBlock.getBuffer() != buffer) {
        uniformBlock.setBuffer(buffer);
        uniformBlock.linkBuffer();
    }
}

void BoneSystemManager::writeBonesForAll() {
    for (auto p : m_buffers)
        writeBones(p.first, p.second);
}

void BoneSystemManager::writeBones(Model *model) {
    if (model->getShape()->isBonesPresent())
        writeBones(model, m_buffers[model]);
}

void BoneSystemManager::linkBuffer(Model *model) {
    if (model->getShape()->isBonesPresent()) {
        linkUniformBuffer(m_uniformBlock, m_buffers[model]);
    } else {
        linkUniformBuffer(m_uniformBlock, m_emptyBuffer);
    }
}

void BoneSystemManager::setupBones(Model *const model) {
    if (model->getShape()->isBonesPresent()) {
        UniformBuffer *buffer = m_buffers[model];
        writeBones(model, buffer);
        linkUniformBuffer(m_uniformBlock, buffer);
    } else {
        linkUniformBuffer(m_uniformBlock, m_emptyBuffer);
    }
}

void BoneSystemManager::destroyBuffer(Model *model) {
    UniformBuffer::destroy(m_buffers[model]);
    m_buffers.erase(model);
}

void BoneSystemManager::destroyBuffers(const std::vector<Model*> &models) {
    for (auto model : models)
        destroyBuffer(model);
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

uint BoneSystemManager::getBindingPoint() const {
    return m_uniformBlock.getBindingPoint();
}

const vector<ShaderProgram*>& BoneSystemManager::getShaderPrograms() const {
    return m_programs;
}

const UniformBlock& BoneSystemManager::getUniformBlock() const {
    return m_uniformBlock;
}

uint BoneSystemManager::getAttribsCount(const uint bonesPerVertex) {
    return bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1);
}

void BoneSystemManager::writeBones(Model *model, UniformBuffer *buffer) {
    const vector<mat4> &bones = *model->getBones();

    buffer->bind();
    buffer->updateData(
            m_uniformBlock.getVarOffset(m_bonesPos),
            sizeof(mat4) * bones.size(), value_ptr(bones[0]));
}
}
