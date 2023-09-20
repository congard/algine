#ifndef ALGINE_BASEUNIFORMBLOCK_H
#define ALGINE_BASEUNIFORMBLOCK_H

#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/UniformBuffer.h>

namespace algine {
class AL_EXPORT BaseUniformBlock {
public:
    static constexpr uint VariableNotFound = -1;

public:
    virtual void init(const ShaderProgram *shaderProgram);

    void assignBindingPoint(const ShaderProgram *shaderProgram) const;
    void allocateSuitableBufferSize();
    void linkBuffer() const;
    void linkBuffer(uint offset, uint size) const;
    void bindBuffer() const;
    void unbindBuffer() const;

    void setBuffer(UniformBuffer *buffer);
    void setName(std::string_view name);
    void setBindingPoint(uint bindingPoint);

    UniformBuffer* getBuffer() const;
    const std::string& getName() const;
    uint getBindingPoint() const;
    uint getIndex(const ShaderProgram *shaderProgram) const;
    uint getSize() const;

public:
    static uint getVarIndex(std::string_view name, const ShaderProgram *shaderProgram);
    static uint getVarOffset(std::string_view name, const ShaderProgram *shaderProgram);
    static bool isVarValid(std::string_view name, const ShaderProgram *shaderProgram);

protected:
    std::string m_name;
    UniformBuffer *m_uniformBuffer;
    uint m_bindingPoint = 0;
    uint m_blockSize;
};
}

#endif //ALGINE_BASEUNIFORMBLOCK_H
