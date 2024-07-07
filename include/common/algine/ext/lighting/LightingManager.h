#ifndef ALGINE_MODULE_H
#define ALGINE_MODULE_H

#include <algine/types.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/shader/UniformBlock.h>
#include <algine/core/buffers/BufferWriter.h>

#include <algine/ext/lighting/Light.h>
#include <algine/ext/lighting/DirLight.h>
#include <algine/ext/lighting/PointLight.h>

#include <tulz/container/Array.h>

namespace algine {
class LightingManager: public Object {
public:
    explicit LightingManager(Object *parent = defaultParent());
    ~LightingManager() override;

    void init();
    void configureShadowMapping();

    void setBindingPoint(uint bindingPoint);
    void setDirLightsLimit(uint limit);
    void setPointLightsLimit(uint limit);
    void setDirMapInitialSlot(uint slot);
    void setPointMapInitialSlot(uint slot);
    void setLightShader(ShaderProgram *lightShader);
    void setPointLightShadowShader(ShaderProgram *shadowShader);

    uint getDirLightsLimit() const;
    uint getPointLightsLimit() const;
    uint getDirMapInitialSlot() const;
    uint getPointMapInitialSlot() const;
    ShaderProgram* getLightShader() const;
    ShaderProgram* getPointLightShadowShader() const;
    const BaseUniformBlock& getUniformBlock() const;

    void bindBuffer() const;
    void unbindBuffer() const;

    // push writes data to the shader

    void pushShadowMap(const DirLight &light, uint index);
    void pushShadowMap(const PointLight &light, uint index);
    void pushShadowShaderPos(const PointLight &light);
    void pushShadowShaderFarPlane(const PointLight &light);
    void pushShadowShaderMatrices(const PointLight &light);

    // write writes data to the buffer

    void writeDirLightsCount(uint count);
    void writePointLightsCount(uint count);

    void writeShadowOpacity(float shadowOpacity);
    void writeShadowDiskRadiusK(float diskRadiusK);
    void writeShadowDiskRadiusMin(float diskRadiusMin);

    void writeKc(const Light &light, uint index);
    void writeKl(const Light &light, uint index);
    void writeKq(const Light &light, uint index);
    void writePos(const Light &light, uint index);
    void writeColor(const Light &light, uint index);

    void writeMinBias(const DirLight &light, uint index);
    void writeMaxBias(const DirLight &light, uint index);
    void writeLightMatrix(const DirLight &light, uint index);

    void writeFarPlane(const PointLight &light, uint index);
    void writeBias(const PointLight &light, uint index);

private:
    struct DirLightOffsets {
        uint kc, kl, kq, pos, color;
        uint minBias, maxBias, lightMatrix;
    };

    struct PointLightOffsets {
        uint kc, kl, kq, pos, color;
        uint far, bias;
    };

    struct LightOffsets {
        tulz::Array<DirLightOffsets> dir;
        tulz::Array<PointLightOffsets> point;

        uint dirCount, pointCount;
        uint shadowOpacity, shadowDiskRadiusK, shadowDiskRadiusMin;
    };

private:
    uint m_dirLightsLimit, m_pointLightsLimit;
    uint m_dirLightsInitialSlot, m_pointLightsInitialSlot;
    ShaderProgram *m_lightShader;
    ShaderProgram *m_pointShadowShader;

private:
    BufferWriter m_bufferWriter;
    BaseUniformBlock m_uniformBlock;
    LightOffsets m_offsets;
    int m_dirShadowMapLoc, m_pointShadowMapLoc;
    int m_shadowShaderPosLoc; // point light shadow shader locations; Loc means Location
    int m_shadowShaderFarPlaneLoc;
    int m_shadowShaderMatricesLoc;
};
}

#endif //ALGINE_MODULE_H
