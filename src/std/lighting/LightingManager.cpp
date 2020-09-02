#include <algine/std/lighting/LightingManager.h>

#include <algine/core/Engine.h>
#include <algine/constants/Lighting.h>
#include <algine/constants/ShadowShader.h>

#include <tulz/macros.h>
#include <glm/gtc/type_ptr.hpp>

namespace LightingVars = algine::Module::Lighting::Vars;
using namespace tulz;
using namespace std;
using namespace glm;

namespace algine {
LightingManager::LightingManager() {
    for (uint & i : m_lightsLimit)
        i = 8; // default lights limit value
}

LightingManager::~LightingManager() {
    delete m_uniformBlock.getBuffer();
}

// for converting enum class to uint
template<typename T> inline uint to_uint(T t) {
    return static_cast<uint>(t);
}

// E means enum
constexpr uint EDirLight = static_cast<uint>(Light::Type::Dir);
constexpr uint EPointLight = static_cast<uint>(Light::Type::Point);

// offsets; used throughout the cpp; to keep things human-readable
enum_class(CommonLightOffsets, Kc, Kl, Kq, Pos, Color, common_elements_count)

enum_class(DirLightOffsets: CommonLightOffsets,
        MinBias = common_elements_count, MaxBias, LightMatrix, elements_count)

enum_class(PointLightOffsets: CommonLightOffsets,
        Far = common_elements_count, Bias, elements_count)

// only for using in init()
#define dirLightElem(elem) \
    LightingVars::Block::DirLights + string("[") + to_string(i) + "]." + LightingVars::Light::elem

#define pointLightElem(elem) \
    LightingVars::Block::PointLights + string("[") + to_string(i) + "]." + LightingVars::Light::elem

#define getDirLightOffset(elem) BaseUniformBlock::getVarOffset(dirLightElem(elem), m_lightShader)
#define getPointLightOffset(elem) BaseUniformBlock::getVarOffset(pointLightElem(elem), m_lightShader)
#define dirLightOffset(type) m_offsets[EDirLight][i][DirLightOffsets::type]
#define pointLightOffset(type) m_offsets[EPointLight][i][PointLightOffsets::type]

void LightingManager::init() {
    m_uniformBlock.setName(LightingVars::Block::Name);
    m_uniformBlock.init(m_lightShader);

    auto uniformBuffer = new UniformBuffer();

    m_bufferWriter.setBuffer(uniformBuffer);

    m_uniformBlock.setBuffer(uniformBuffer);
    m_uniformBlock.allocateSuitableBufferSize();
    m_uniformBlock.assignBindingPoint(m_lightShader);
    m_uniformBlock.linkBuffer();

    m_lightsCountOffset[EDirLight] = BaseUniformBlock::getVarOffset(
            LightingVars::Block::DirLightsCount, m_lightShader);
    m_lightsCountOffset[EPointLight] = BaseUniformBlock::getVarOffset(
            LightingVars::Block::PointLightsCount, m_lightShader);
    m_shadowMapsLocations[EDirLight] = m_lightShader->getLocation(LightingVars::DirLightShadowMaps);
    m_shadowMapsLocations[EPointLight] = m_lightShader->getLocation(LightingVars::PointLightShadowMaps);

    m_shadowOpacityOffset = BaseUniformBlock::getVarOffset(
            LightingVars::Block::ShadowOpacity, m_lightShader);
    m_shadowDiskRadiusKOffset = BaseUniformBlock::getVarOffset(
            LightingVars::Block::ShadowDiskRadiusK, m_lightShader);
    m_shadowDiskRadiusMinOffset = BaseUniformBlock::getVarOffset(
            LightingVars::Block::ShadowDiskRadiusMin, m_lightShader);

    if (m_pointShadowShader) {
        m_shadowShaderPosLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::Pos);
        m_shadowShaderFarPlaneLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::FarPlane);
        m_shadowShaderMatricesLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::ShadowMatrices);
    }

    m_offsets[EDirLight] = Array<LightOffsets>(m_lightsLimit[EDirLight]);
    m_offsets[EPointLight] = Array<LightOffsets>(m_lightsLimit[EPointLight]);

    // dir light offsets
    for (uint i = 0; i < m_lightsLimit[EDirLight]; ++i) {
        m_offsets[EDirLight][i] = LightOffsets(DirLightOffsets::elements_count);
        dirLightOffset(Kc) = getDirLightOffset(Kc);
        dirLightOffset(Kl) = getDirLightOffset(Kl);
        dirLightOffset(Kq) = getDirLightOffset(Kq);
        dirLightOffset(Pos) = getDirLightOffset(Pos);
        dirLightOffset(Color) = getDirLightOffset(Color);
        dirLightOffset(MinBias) = getDirLightOffset(MinBias);
        dirLightOffset(MaxBias) = getDirLightOffset(MaxBias);
        dirLightOffset(LightMatrix) = getDirLightOffset(LightMatrix);
    }

    // point light offsets
    for (uint i = 0; i < m_lightsLimit[EPointLight]; ++i) {
        m_offsets[EPointLight][i] = LightOffsets(PointLightOffsets::elements_count);
        pointLightOffset(Kc) = getPointLightOffset(Kc);
        pointLightOffset(Kl) = getPointLightOffset(Kl);
        pointLightOffset(Kq) = getPointLightOffset(Kq);
        pointLightOffset(Pos) = getPointLightOffset(Pos);
        pointLightOffset(Color) = getPointLightOffset(Color);
        pointLightOffset(Far) = getPointLightOffset(FarPlane);
        pointLightOffset(Bias) = getPointLightOffset(Bias);
    }
}

void LightingManager::configureShadowMapping() {
    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    // Note: Mesa drivers require sampler as int, not uint

    for (int i = 0; i < m_lightsLimit[EDirLight]; i++) {
        ShaderProgram::setInt(m_shadowMapsLocations[EDirLight] + i,
                              static_cast<int>(m_lightsInitialSlot[EDirLight] + i));
        Engine::defaultTexture2D()->use(m_lightsInitialSlot[EDirLight] + i);
    }

    for (int i = 0; i < m_lightsLimit[EPointLight]; i++) {
        ShaderProgram::setInt(m_shadowMapsLocations[EPointLight] + i,
                              static_cast<int>(m_lightsInitialSlot[EPointLight] + i));
        Engine::defaultTextureCube()->use(m_lightsInitialSlot[EPointLight] + i);
    }
}

void LightingManager::setBindingPoint(uint bindingPoint) {
    m_uniformBlock.setBindingPoint(bindingPoint);
}

void LightingManager::setLightsLimit(uint limit, Light::Type lightType) {
    m_lightsLimit[to_uint(lightType)] = limit;
}

void LightingManager::setLightsMapInitialSlot(uint slot, Light::Type lightType) {
    m_lightsInitialSlot[to_uint(lightType)] = slot;
}

void LightingManager::setLightShader(ShaderProgram *const in_lightShader) {
    m_lightShader = in_lightShader;
}

void LightingManager::setPointLightShadowShader(ShaderProgram *shadowShader) {
    m_pointShadowShader = shadowShader;
}

uint LightingManager::getLightsLimit(Light::Type lightType) const {
    return m_lightsLimit[to_uint(lightType)];
}

uint LightingManager::getLightsMapInitialSlot(Light::Type lightType) const {
    return m_lightsInitialSlot[to_uint(lightType)];
}

ShaderProgram* LightingManager::getLightShader() const {
    return m_lightShader;
}

ShaderProgram* LightingManager::getPointLightShadowShader() const {
    return m_pointShadowShader;
}

const BaseUniformBlock& LightingManager::getUniformBlock() const {
    return m_uniformBlock;
}

void LightingManager::bindBuffer() const {
    m_uniformBlock.bindBuffer();
}

void LightingManager::unbindBuffer() const {
    m_uniformBlock.unbindBuffer();
}

void LightingManager::pushShadowMap(const DirLight &light, uint index) {
    uint slot = m_lightsInitialSlot[EDirLight] + index;
    ShaderProgram::setInt(m_shadowMapsLocations[EDirLight] + static_cast<int>(index), static_cast<int>(slot));
    light.m_shadowMap->use(slot);
}

void LightingManager::pushShadowMap(const PointLight &light, uint index) {
    uint slot = m_lightsInitialSlot[EPointLight] + index;
    ShaderProgram::setInt(m_shadowMapsLocations[EPointLight] + static_cast<int>(index), static_cast<int>(slot));
    light.m_shadowMap->use(slot);
}

void LightingManager::pushShadowShaderPos(const PointLight &light) {
    ShaderProgram::setVec3(m_shadowShaderPosLoc, light.m_pos);
}

void LightingManager::pushShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::setFloat(m_shadowShaderFarPlaneLoc, light.m_far);
}

void LightingManager::pushShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++)
        ShaderProgram::setMat4(m_shadowShaderMatricesLoc + i, light.m_lightSpaceMatrices[i]);
}

void LightingManager::writeDirLightsCount(uint count) {
    m_bufferWriter.write(m_lightsCountOffset[EDirLight], count);
}

void LightingManager::writePointLightsCount(uint count) {
    m_bufferWriter.write(m_lightsCountOffset[EPointLight], count);
}

void LightingManager::writeShadowOpacity(float shadowOpacity) {
    m_bufferWriter.write(m_shadowOpacityOffset, shadowOpacity);
}

void LightingManager::writeShadowDiskRadiusK(float diskRadiusK) {
    m_bufferWriter.write(m_shadowDiskRadiusKOffset, diskRadiusK);
}

void LightingManager::writeShadowDiskRadiusMin(float diskRadiusMin) {
    m_bufferWriter.write(m_shadowDiskRadiusMinOffset, diskRadiusMin);
}

#define lightOffsets m_offsets[to_uint(light.m_type)][index]

void LightingManager::writeKc(const Light &light, uint index) {
    m_bufferWriter.write(lightOffsets[CommonLightOffsets::Kc], light.m_kc);
}

void LightingManager::writeKl(const Light &light, uint index) {
    m_bufferWriter.write(lightOffsets[CommonLightOffsets::Kl], light.m_kl);
}

void LightingManager::writeKq(const Light &light, uint index) {
    m_bufferWriter.write(lightOffsets[CommonLightOffsets::Kq], light.m_kq);
}

void LightingManager::writePos(const Light &light, uint index) {
    m_bufferWriter.write(lightOffsets[CommonLightOffsets::Pos], light.m_pos);
}

void LightingManager::writeColor(const Light &light, uint index) {
    m_bufferWriter.write(lightOffsets[CommonLightOffsets::Color], light.m_color);
}

void LightingManager::writeMinBias(const DirLight &light, uint index) {
    m_bufferWriter.write(lightOffsets[DirLightOffsets::MinBias], light.m_minBias);
}

void LightingManager::writeMaxBias(const DirLight &light, uint index) {
    m_bufferWriter.write(lightOffsets[DirLightOffsets::MaxBias], light.m_maxBias);
}

void LightingManager::writeLightMatrix(const DirLight &light, uint index) {
    m_bufferWriter.write(lightOffsets[DirLightOffsets::LightMatrix], light.m_lightSpace);
}

void LightingManager::writeFarPlane(const PointLight &light, uint index) {
    m_bufferWriter.write(lightOffsets[PointLightOffsets::Far], light.m_far);
}

void LightingManager::writeBias(const PointLight &light, uint index) {
    m_bufferWriter.write(lightOffsets[PointLightOffsets::Bias], light.m_bias);
}
}