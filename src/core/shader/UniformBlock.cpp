#include <algine/core/shader/UniformBlock.h>

#include <algine/core/Engine.h>

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

using namespace std;
using namespace tulz;
using namespace glm;

namespace algine {
void UniformBlock::init() {
    load();
    configureBuffer();
    linkBuffer();
}

void UniformBlock::load() {
    m_varIndices = Array<uint>(m_varNames.size());
    m_varOffsets = Array<uint>(m_varNames.size());

    m_blockIndex = glGetUniformBlockIndex(m_shaderProgram->id, m_name.c_str());
    glGetActiveUniformBlockiv(m_shaderProgram->id, m_blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE,
                              reinterpret_cast<int*>(&m_blockSize));

    for (uint i = 0; i < m_varNames.size(); i++) {
        auto varName = m_varNames[i].c_str();
        glGetUniformIndices(m_shaderProgram->id, 1, &varName, &m_varIndices[i]);

        // If skip this check and the GLSL compiler removes the variable (if it unused, e.g in packed format),
        // then there will be GL_INVALID_VALUE: "A uniform index exceeds the total number of uniforms."
        // Since m_varIndices[i] will contain VariableNotFound
        if (m_varIndices[i] != VariableNotFound) {
            int offset;
            glGetActiveUniformsiv(m_shaderProgram->id, 1, &m_varIndices[i], GL_UNIFORM_OFFSET, &offset);
            m_varOffsets[i] = offset;
        }
    }
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
    glBindBufferBase(GL_UNIFORM_BUFFER, m_blockIndex, m_uniformBuffer->m_id);
    Engine::defaultUniformBuffer()->bind();
}

void UniformBlock::linkBuffer(const uint offset, const uint size) {
    glBindBufferRange(GL_UNIFORM_BUFFER, m_blockIndex, m_uniformBuffer->m_id, offset, size);
    Engine::defaultUniformBuffer()->bind();
}

void UniformBlock::setShaderProgram(ShaderProgram *shaderProgram) {
    m_shaderProgram = shaderProgram;
}

void UniformBlock::setBuffer(UniformBuffer *buffer) {
    m_uniformBuffer = buffer;
}

void UniformBlock::setName(const string &name) {
    m_name = name;
}

void UniformBlock::setVarNames(const vector<string> &names) {
    m_varNames = Array<string>(names.size());

    for (uint i = 0; i < names.size(); i++)
        m_varNames[i] = names[i];
}

void UniformBlock::setVarNames(const Array<string> &names) {
    m_varNames = names;
}

ShaderProgram* UniformBlock::getShaderProgram() const {
    return m_shaderProgram;
}

UniformBuffer* UniformBlock::getBuffer() const {
    return m_uniformBuffer;
}

string UniformBlock::getName() const {
    return m_name;
}

uint UniformBlock::getIndex() const {
    return m_blockIndex;
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

Array<uint> UniformBlock::getVarIndices() const {
    return m_varIndices;
}

Array<uint> UniformBlock::getVarOffsets() const {
    return m_varOffsets;
}

uint UniformBlock::getVarIndex(const string &name) const {
    return m_varIndices.array()[getVarPosition(name)];
}

uint UniformBlock::getVarOffset(const string &name) const {
    return m_varOffsets.array()[getVarPosition(name)];
}

uint UniformBlock::getVarIndex(const uint position) const {
    return m_varIndices.array()[position];
}

uint UniformBlock::getVarOffset(const uint position) const {
    return m_varOffsets.array()[position];
}

bool UniformBlock::isVarValid(const string &name) const {
    return getVarIndex(name) != VariableNotFound;
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
}
