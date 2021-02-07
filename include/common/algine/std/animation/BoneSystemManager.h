#ifndef ALGINE_BONESYSTEMMANAGER_H
#define ALGINE_BONESYSTEMMANAGER_H

#include <algine/core/shader/UniformBlock.h>
#include <algine/core/buffers/BlockBufferStorage.h>

#include <algine/std/model/ModelPtr.h>

#include <unordered_map>

namespace algine {
class BoneSystemManager {
public:
    void init();
    void writeBonesForAll();
    void writeBones(const ModelPtr &model);
    void linkBuffer(const ModelPtr &model);
    void setupBones(const ModelPtr &model);

    void setMaxModelsCount(uint count);
    void setBindingPoint(uint bindingPoint);
    void setShaderPrograms(const std::vector<ShaderProgramPtr> &shaderPrograms);
    void addShaderProgram(const ShaderProgramPtr &shaderProgram);
    void addModel(const ModelPtr &model);
    void addModels(const std::vector<ModelPtr> &models);

    void removeModel(const ModelPtr &model);
    void removeModels(const std::vector<ModelPtr> &models);

    uint getBindingPoint() const;
    const std::vector<ShaderProgramPtr>& getShaderPrograms() const;
    const UniformBlock& getUniformBlock() const;
    const BlockBufferStorage& getBlockBufferStorage() const;

    static uint getAttribsCount(uint bonesPerVertex);

private:
    void writeBones(const ModelPtr &model, Index index);
    void linkUniformBuffer(Index blockIndex);

private:
    std::vector<ShaderProgramPtr> m_programs;
    std::unordered_map<ModelPtr, uint> m_ids;
    BlockBufferStorage m_bufferStorage;
    UniformBlock m_uniformBlock;
    uint m_bonesPos, m_boneAttribsCountPos, m_linkedBlock = -1;
};
}

#endif //ALGINE_BONESYSTEMMANAGER_H
