#ifndef ALGINE_BONESYSTEMMANAGER_H
#define ALGINE_BONESYSTEMMANAGER_H

#include <algine/core/shader/UniformBlock.h>
#include <algine/core/buffers/BlockBufferStorage.h>

#include <unordered_map>

namespace algine {
class Model;

class BoneSystemManager {
public:
    void init(Object *bufferParent);
    void writeBonesForAll();
    void writeBones(Model *model);
    void linkBuffer(Model *model);
    void setupBones(Model *model);

    void setMaxModelsCount(uint count);
    void setBindingPoint(uint bindingPoint);
    void setShaderPrograms(const std::vector<ShaderProgram*> &shaderPrograms);
    void addShaderProgram(ShaderProgram *shaderProgram);
    void addModel(Model *model);
    void addModels(const std::vector<Model*> &models);

    void removeModel(Model *model);
    void removeModels(const std::vector<Model*> &models);

    uint getBindingPoint() const;
    const std::vector<ShaderProgram*>& getShaderPrograms() const;
    const UniformBlock& getUniformBlock() const;
    const BlockBufferStorage& getBlockBufferStorage() const;

    static uint getAttribsCount(uint bonesPerVertex);

private:
    void writeBones(Model *model, Index index);
    void linkUniformBuffer(Index blockIndex);

private:
    std::vector<ShaderProgram*> m_programs;
    std::unordered_map<Model*, uint> m_ids;
    BlockBufferStorage m_bufferStorage;
    UniformBlock m_uniformBlock;
    uint m_bonesPos, m_boneAttribsCountPos, m_linkedBlock = -1;
};
}

#endif //ALGINE_BONESYSTEMMANAGER_H
