#ifndef ALGINE_BASEUNIFORMBLOCK_H
#define ALGINE_BASEUNIFORMBLOCK_H

#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/UniformBuffer.h>

namespace algine {
class BaseUniformBlock {
public:
    static constexpr uint VariableNotFound = -1;

public:
    virtual void init(const ShaderProgram *shaderProgram);

    void assignBindingPoint(const ShaderProgram *shaderProgram) const;
    void allocateSuitableBufferSize();
    void linkBuffer();
    void linkBuffer(uint offset, uint size);

    void setBuffer(UniformBuffer *buffer);
    void setName(const std::string &name);
    void setBindingPoint(uint bindingPoint);

    UniformBuffer* getBuffer() const;
    std::string getName() const;
    uint getBindingPoint() const;
    uint getIndex(const ShaderProgram *shaderProgram) const;
    uint getSize() const;

public:
    static uint getVarIndex(const std::string &name, const ShaderProgram *shaderProgram);
    static uint getVarOffset(const std::string &name, const ShaderProgram *shaderProgram);
    static bool isVarValid(const std::string &name, const ShaderProgram *shaderProgram);

protected:
    std::string m_name;
    UniformBuffer *m_uniformBuffer;
    uint m_bindingPoint = 0, m_blockSize;
};
}

#endif //ALGINE_BASEUNIFORMBLOCK_H
