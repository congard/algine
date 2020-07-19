#ifndef ALGINE_UNIFORMBLOCK_H
#define ALGINE_UNIFORMBLOCK_H

#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/UniformBuffer.h>

#include <tulz/Array.h>

namespace algine {
class UniformBlock {
public:
    static constexpr uint PositionNotFound = -1;
    static constexpr uint VariableNotFound = -1;

public:
    void init();

    void load();
    void configureBuffer();
    void linkBuffer();
    void linkBuffer(uint offset, uint size);

    void setShaderProgram(ShaderProgram *shaderProgram);
    void setBuffer(UniformBuffer *buffer);
    void setName(const std::string &name);
    void setVarNames(const std::vector<std::string> &names);
    void setVarNames(const tulz::Array<std::string> &names);

    ShaderProgram* getShaderProgram() const;
    UniformBuffer* getBuffer() const;
    std::string getName() const;
    uint getIndex() const;
    uint getSize() const;
    uint getVarPosition(const std::string &name) const;
    tulz::Array<std::string> getVarNames() const;
    tulz::Array<uint> getVarIndices() const;
    tulz::Array<uint> getVarOffsets() const;
    uint getVarIndex(const std::string &name) const;
    uint getVarOffset(const std::string &name) const;
    uint getVarIndex(uint position) const;
    uint getVarOffset(uint position) const;

    bool isVarValid(const std::string &name) const;

    void write(uint position, uint size, const void *data);
    void writeBool(uint position, bool p);
    void writeInt(uint position, int p);
    void writeUint(uint position, uint p);
    void writeFloat(uint position, float p);
    void writeVec3(uint position, const glm::vec3 &p);
    void writeVec4(uint position, const glm::vec4 &p);
    void writeMat3(uint position, const glm::mat3 &p);
    void writeMat4(uint position, const glm::mat4 &p);

    void write(const std::string &name, uint size, const void *data);
    void writeBool(const std::string &name, bool p);
    void writeInt(const std::string &name, int p);
    void writeUint(const std::string &name, uint p);
    void writeFloat(const std::string &name, float p);
    void writeVec3(const std::string &name, const glm::vec3 &p);
    void writeVec4(const std::string &name, const glm::vec4 &p);
    void writeMat3(const std::string &name, const glm::mat3 &p);
    void writeMat4(const std::string &name, const glm::mat4 &p);

private:
    std::string m_name;

    tulz::Array<std::string> m_varNames;
    tulz::Array<uint> m_varIndices;
    tulz::Array<uint> m_varOffsets;

    ShaderProgram *m_shaderProgram;
    UniformBuffer *m_uniformBuffer;
    uint m_blockIndex, m_blockSize;
};
}

#endif //ALGINE_UNIFORMBLOCK_H
