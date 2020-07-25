#ifndef ALGINE_BONESYSTEMMANAGER_H
#define ALGINE_BONESYSTEMMANAGER_H

#include <algine/core/shader/UniformBlock.h>
#include <algine/std/model/Model.h>

#include <unordered_map>

namespace algine {
class BoneSystemManager {
public:
    ~BoneSystemManager();

    void init();
    void initBuffer(Model *model);
    void initBuffers(const std::vector<Model*> &models);
    void writeBones(Model *model);
    void linkBuffer(Model *model);
    void setupBones(Model *model);

    void destroyBuffer(Model *model);
    void destroyBuffers(const std::vector<Model*> &models);

    void setBindingPoint(uint bindingPoint);
    void setShaderPrograms(const std::vector<ShaderProgram*> &shaderPrograms);
    void addShaderProgram(ShaderProgram* shaderProgram);

    uint getBindingPoint() const;
    const std::vector<ShaderProgram*>& getShaderPrograms() const;
    const UniformBlock& getUniformBlock() const;

    static uint getAttribsCount(uint bonesPerVertex);

private:
    void writeBones(Model *model, UniformBuffer *buffer);

private:
    std::vector<ShaderProgram*> m_programs;
    std::unordered_map<Model*, UniformBuffer*> m_buffers;
    UniformBuffer* m_emptyBuffer;
    UniformBlock m_uniformBlock;
    uint m_bonesPos, m_boneAttribsCountPos;
};
}

#endif //ALGINE_BONESYSTEMMANAGER_H
