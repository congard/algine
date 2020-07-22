#include <algine/core/shader/UniformBlock.h>

#include <algine/core/Engine.h>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace std;
using namespace tulz;
using namespace glm;

namespace algine {
void UniformBlock::assignBindingPoint(const ShaderProgram *shaderProgram) const {
    glUniformBlockBinding(shaderProgram->id, getIndex(shaderProgram), m_bindingPoint);
}

void UniformBlock::configureBuffer() {
    m_uniformBuffer->bind();
    m_uniformBuffer->setData(m_blockSize, nullptr, Buffer::DynamicDraw);
    m_uniformBuffer->unbind();
}

// Engine::defaultUniformBuffer()->bind() because:
// glBindBufferBase also binds buffer to the generic buffer binding point specified by target
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml

void UniformBlock::linkBuffer() {
    glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_uniformBuffer->m_id);
    Engine::defaultUniformBuffer()->bind();
}

void UniformBlock::linkBuffer(const uint offset, const uint size) {
    glBindBufferRange(GL_UNIFORM_BUFFER, m_bindingPoint, m_uniformBuffer->m_id, offset, size);
    Engine::defaultUniformBuffer()->bind();
}

void UniformBlock::loadBlockInfo(const ShaderProgram *shaderProgram) {
    m_varOffsets = Array<uint>(m_varNames.size());

    const uint blockIndex = getIndex(shaderProgram);
    glGetActiveUniformBlockiv(shaderProgram->id, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE,
                              reinterpret_cast<int*>(&m_blockSize));

    for (uint i = 0; i < m_varNames.size(); i++) {
        uint varIndex = getVarIndex(shaderProgram, m_varNames[i]);

        // If skip this check and the GLSL compiler removes the variable (if it unused, e.g in packed format),
        // then there will be GL_INVALID_VALUE: "A uniform index exceeds the total number of uniforms."
        // Since varIndex will contain VariableNotFound
        if (varIndex != VariableNotFound) {
            int offset;
            glGetActiveUniformsiv(shaderProgram->id, 1, &varIndex, GL_UNIFORM_OFFSET, &offset);
            m_varOffsets[i] = offset;
        }
    }
}

void UniformBlock::setBuffer(UniformBuffer *buffer) {
    m_uniformBuffer = buffer;
}

void UniformBlock::setName(const string &name) {
    m_name = name;
}

void UniformBlock::setBindingPoint(const uint bindingPoint) {
    m_bindingPoint = bindingPoint;
}

void UniformBlock::setBlockInfo(const uint size, const Array<uint> &varOffsets) {
    m_blockSize = size;
    m_varOffsets = varOffsets;
}

void UniformBlock::setBlockInfo(const uint size, const vector<uint> &varOffsets) {
    m_blockSize = size;
    m_varOffsets = Array<uint>(varOffsets.size());

    for (uint i = 0; i < varOffsets.size(); i++)
        m_varOffsets[i] = varOffsets[i];
}

void UniformBlock::setVarNames(const vector<string> &names) {
    m_varNames = Array<string>(names.size());

    for (uint i = 0; i < names.size(); i++)
        m_varNames[i] = names[i];
}

void UniformBlock::setVarNames(const Array<string> &names) {
    m_varNames = names;
}

UniformBuffer* UniformBlock::getBuffer() const {
    return m_uniformBuffer;
}

string UniformBlock::getName() const {
    return m_name;
}

uint UniformBlock::getBindingPoint() const {
    return m_bindingPoint;
}

uint UniformBlock::getIndex(const ShaderProgram *shaderProgram) const {
    return glGetUniformBlockIndex(shaderProgram->id, m_name.c_str());;
}

uint UniformBlock::getSize() const {
    return m_blockSize;
}

uint UniformBlock::getVarPosition(const string &name) const {
    for (uint i = 0; i < m_varNames.size(); i++)
        if (m_varNames.array()[i] == name)
            return i;

    return PositionNotFound;
}

Array<string> UniformBlock::getVarNames() const {
    return m_varNames;
}

Array<uint> UniformBlock::getVarOffsets() const {
    return m_varOffsets;
}

uint UniformBlock::getVarOffset(const string &name) const {
    return m_varOffsets.array()[getVarPosition(name)];
}

uint UniformBlock::getVarOffset(const uint position) const {
    return m_varOffsets.array()[position];
}

// Definitions are not generated to keep things simple (first of all debugging)

void UniformBlock::write(uint position, uint size, const void *data) {
    m_uniformBuffer->updateData(m_varOffsets[position], size, data);
}

void UniformBlock::writeBool(uint position, bool p) {
    write(position, sizeof(p), &p);
}

void UniformBlock::writeInt(uint position, int p) {
    write(position, sizeof(p), &p);
}

void UniformBlock::writeUint(uint position, uint p) {
    write(position, sizeof(p), &p);
}

void UniformBlock::writeFloat(uint position, float p) {
    write(position, sizeof(p), &p);
}

void UniformBlock::writeVec3(uint position, const vec3 &p) {
    write(position, sizeof(p), value_ptr(p));
}

void UniformBlock::writeVec4(uint position, const vec4 &p) {
    write(position, sizeof(p), value_ptr(p));
}

void UniformBlock::writeMat3(uint position, const mat3 &p) {
    write(position, sizeof(p), value_ptr(p));
}

void UniformBlock::writeMat4(uint position, const mat4 &p) {
    write(position, sizeof(p), value_ptr(p));
}

void UniformBlock::write(const string &name, uint size, const void *data) {
    write(getVarPosition(name), size, data);
}

void UniformBlock::writeBool(const string &name, bool p) {
    writeBool(getVarPosition(name), p);
}

void UniformBlock::writeInt(const string &name, int p) {
    writeInt(getVarPosition(name), p);
}

void UniformBlock::writeUint(const string &name, uint p) {
    writeUint(getVarPosition(name), p);
}

void UniformBlock::writeFloat(const string &name, float p) {
    writeFloat(getVarPosition(name), p);
}

void UniformBlock::writeVec3(const string &name, const vec3 &p) {
    writeVec3(getVarPosition(name), p);
}

void UniformBlock::writeVec4(const string &name, const vec4 &p) {
    writeVec4(getVarPosition(name), p);
}

void UniformBlock::writeMat3(const string &name, const mat3 &p) {
    writeMat3(getVarPosition(name), p);
}

void UniformBlock::writeMat4(const string &name, const mat4 &p) {
    writeMat4(getVarPosition(name), p);
}

uint UniformBlock::getVarIndex(const ShaderProgram *shaderProgram, const std::string &name) {
    uint index;
    auto varName = name.c_str();
    glGetUniformIndices(shaderProgram->id, 1, &varName, &index);

    return index;
}

bool UniformBlock::isVarValid(const ShaderProgram *shaderProgram, const std::string &name) {
    return getVarIndex(shaderProgram, name) != VariableNotFound;
}
}
