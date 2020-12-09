#ifndef ALGINE_MODULE_H
#define ALGINE_MODULE_H

#include <algine/types.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/shader/UniformBlock.h>
#include <algine/core/buffers/BufferWriter.h>

#include <algine/std/lighting/Light.h>
#include <algine/std/lighting/DirLight.h>
#include <algine/std/lighting/PointLight.h>

#include <tulz/Array.h>

namespace algine {
class LightingManager {
public:
    LightingManager();
    ~LightingManager();

    void init();
    void configureShadowMapping();

    void setBindingPoint(uint bindingPoint);
    void setLightsLimit(uint limit, Light::Type lightType);
    void setLightsMapInitialSlot(uint slot, Light::Type lightType);
    void setLightShader(const ShaderProgramPtr &lightShader);
    void setPointLightShadowShader(const ShaderProgramPtr &shadowShader);

    uint getLightsLimit(Light::Type lightType) const;
    uint getLightsMapInitialSlot(Light::Type lightType) const;
    const ShaderProgramPtr& getLightShader() const;
    const ShaderProgramPtr& getPointLightShadowShader() const;
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
    typedef tulz::Array<uint> LightOffsets;

private:
    uint m_lightsLimit[Light::TypesCount];
    uint m_lightsInitialSlot[Light::TypesCount];
    ShaderProgramPtr m_lightShader;
    ShaderProgramPtr m_pointShadowShader;

private:
    BufferWriter m_bufferWriter;
    BaseUniformBlock m_uniformBlock;
    tulz::Array<LightOffsets> m_offsets[Light::TypesCount];
    uint m_lightsCountOffset[Light::TypesCount];
    uint m_shadowOpacityOffset, m_shadowDiskRadiusKOffset, m_shadowDiskRadiusMinOffset;
    int m_shadowMapsLocations[Light::TypesCount];
    int m_shadowShaderPosLoc; // point light shadow shader locations; Loc means Location
    int m_shadowShaderFarPlaneLoc;
    int m_shadowShaderMatricesLoc;
};
}

#endif //ALGINE_MODULE_H
