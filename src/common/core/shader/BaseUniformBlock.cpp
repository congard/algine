#include <algine/core/shader/BaseUniformBlock.h>
#include <algine/core/Engine.h>
#include <algine/gl.h>

namespace algine {
void BaseUniformBlock::init(const ShaderProgram *shaderProgram) {
    const uint blockIndex = getIndex(shaderProgram);
    glGetActiveUniformBlockiv(shaderProgram->getId(), blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE,
                              reinterpret_cast<int*>(&m_blockSize));
}

void BaseUniformBlock::assignBindingPoint(const ShaderProgram *shaderProgram) const {
    glUniformBlockBinding(shaderProgram->getId(), getIndex(shaderProgram), m_bindingPoint);
}

void BaseUniformBlock::allocateSuitableBufferSize() {
    m_uniformBuffer->bind();
    m_uniformBuffer->setData(m_blockSize, nullptr, Buffer::DynamicDraw);
    m_uniformBuffer->unbind();
}

// Engine::defaultUniformBuffer()->bind() because:
// glBindBufferBase also binds buffer to the generic buffer binding point specified by target
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml

void BaseUniformBlock::linkBuffer() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_uniformBuffer->m_id);
    Engine::defaultUniformBuffer()->bind();
}

void BaseUniformBlock::linkBuffer(const uint offset, const uint size) const {
    glBindBufferRange(GL_UNIFORM_BUFFER, m_bindingPoint, m_uniformBuffer->m_id, offset, size);
    Engine::defaultUniformBuffer()->bind();
}

void BaseUniformBlock::bindBuffer() const {
    m_uniformBuffer->bind();
}

void BaseUniformBlock::unbindBuffer() const {
    m_uniformBuffer->unbind();
}

void BaseUniformBlock::setBuffer(UniformBuffer *buffer) {
    m_uniformBuffer = buffer;
}

void BaseUniformBlock::setName(std::string_view name) {
    m_name = name;
}

void BaseUniformBlock::setBindingPoint(const uint bindingPoint) {
    m_bindingPoint = bindingPoint;
}


UniformBuffer* BaseUniformBlock::getBuffer() const {
    return m_uniformBuffer;
}

const std::string& BaseUniformBlock::getName() const {
    return m_name;
}

uint BaseUniformBlock::getBindingPoint() const {
    return m_bindingPoint;
}

uint BaseUniformBlock::getIndex(const ShaderProgram *shaderProgram) const {
    return glGetUniformBlockIndex(shaderProgram->getId(), m_name.c_str());;
}

uint BaseUniformBlock::getSize() const {
    return m_blockSize;
}

uint BaseUniformBlock::getVarIndex(std::string_view name, const ShaderProgram *shaderProgram) {
    uint index;
    auto varName = name.data();
    glGetUniformIndices(shaderProgram->getId(), 1, &varName, &index);
    return index;
}

uint BaseUniformBlock::getVarOffset(std::string_view name, const ShaderProgram *shaderProgram) {
    uint varIndex = getVarIndex(name, shaderProgram);

    // If skip this check and the GLSL compiler removes the variable (if it unused, e.g in packed format),
    // then there will be GL_INVALID_VALUE: "A uniform index exceeds the total number of uniforms."
    // Since varIndex will contain VariableNotFound
    if (varIndex != VariableNotFound) {
        int offset;
        glGetActiveUniformsiv(shaderProgram->getId(), 1, &varIndex, GL_UNIFORM_OFFSET, &offset);
        return offset;
    }

    return VariableNotFound;
}

bool BaseUniformBlock::isVarValid(std::string_view name, const ShaderProgram *shaderProgram) {
    return getVarIndex(name, shaderProgram) != VariableNotFound;
}
}
