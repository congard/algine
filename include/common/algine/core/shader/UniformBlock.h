#ifndef ALGINE_UNIFORMBLOCK_H
#define ALGINE_UNIFORMBLOCK_H

#include <algine/core/shader/BaseUniformBlock.h>

#include <tulz/Array.h>

namespace algine {
class UniformBlock: public BaseUniformBlock {
public:
    static constexpr uint PositionNotFound = -1;

public:
    void init(const ShaderProgram *shaderProgram) override;

    void setBlockInfo(uint size, const tulz::Array<uint>& varOffsets);
    void setVarNames(const tulz::Array<std::string> &names);

    uint getVarPosition(const std::string &name) const;
    const tulz::Array<std::string>& getVarNames() const;
    std::string getVarName(uint position) const;
    const tulz::Array<uint>& getVarOffsets() const;
    uint getVarOffset(const std::string &name) const;
    uint getVarOffset(uint position) const;
    tulz::Array<uint> getVarIndices(const ShaderProgram *shaderProgram) const;
    uint getVarIndex(uint position, const ShaderProgram *shaderProgram) const;

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
    tulz::Array<std::string> m_varNames;
    tulz::Array<uint> m_varOffsets;
};
}

#endif //ALGINE_UNIFORMBLOCK_H
