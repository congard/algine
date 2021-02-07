#include <algine/core/shader/UniformBlock.h>

#include <algine/core/Engine.h>

#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace tulz;
using namespace glm;

namespace algine {
void UniformBlock::init(const ShaderProgram *shaderProgram) {
    BaseUniformBlock::init(shaderProgram);

    m_varOffsets = Array<uint>(m_varNames.size());

    for (uint i = 0; i < m_varNames.size(); i++)
        m_varOffsets[i] = BaseUniformBlock::getVarOffset(m_varNames[i], shaderProgram);
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

uint UniformBlock::getVarPosition(const string &name) const {
    for (uint i = 0; i < m_varNames.size(); i++)
        if (m_varNames[i] == name)
            return i;

    return PositionNotFound;
}

const Array<string>& UniformBlock::getVarNames() const {
    return m_varNames;
}

string UniformBlock::getVarName(uint position) const {
    return m_varNames[position];
}

const Array<uint>& UniformBlock::getVarOffsets() const {
    return m_varOffsets;
}

uint UniformBlock::getVarOffset(const string &name) const {
    return m_varOffsets[getVarPosition(name)];
}

uint UniformBlock::getVarOffset(uint position) const {
    return m_varOffsets[position];
}

Array<uint> UniformBlock::getVarIndices(const ShaderProgram *shaderProgram) const {
    auto indices = Array<uint>(m_varNames.size());

    for (uint i = 0; i < indices.size(); i++)
        indices[i] = BaseUniformBlock::getVarIndex(m_varNames[i], shaderProgram);

    return indices;
}

uint UniformBlock::getVarIndex(uint position, const ShaderProgram *shaderProgram) const {
    return BaseUniformBlock::getVarIndex(m_varNames[position], shaderProgram);
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
