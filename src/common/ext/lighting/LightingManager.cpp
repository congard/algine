#include <algine/ext/lighting/LightingManager.h>

#include <algine/core/Engine.h>
#include <algine/ext/lighting/LightingConstants.h>
#include <algine/constants/ShadowShader.h>

using namespace tulz;
using namespace std;
using namespace glm;

namespace algine {
LightingManager::LightingManager()
    : m_dirLightsLimit(8),
      m_pointLightsLimit(8),
      m_dirLightsInitialSlot(),
      m_pointLightsInitialSlot(),
      m_lightShader(),
      m_pointShadowShader(),
      m_dirShadowMapLoc(),
      m_pointShadowMapLoc(),
      m_shadowShaderPosLoc(-1),
      m_shadowShaderFarPlaneLoc(-1),
      m_shadowShaderMatricesLoc(-1) {}

LightingManager::~LightingManager() {
    delete m_uniformBlock.getBuffer();
}

void LightingManager::init() {
    namespace LightingVars = algine::Module::Lighting::Vars;

    const auto lightShaderRaw = m_lightShader.get();

    m_uniformBlock.setName(LightingVars::Block::Name);
    m_uniformBlock.init(lightShaderRaw);

    auto uniformBuffer = new UniformBuffer();

    m_bufferWriter.setBuffer(uniformBuffer);

    m_uniformBlock.setBuffer(uniformBuffer);
    m_uniformBlock.allocateSuitableBufferSize();
    m_uniformBlock.assignBindingPoint(lightShaderRaw);
    m_uniformBlock.linkBuffer();

    m_offsets.dirCount = BaseUniformBlock::getVarOffset(LightingVars::Block::DirLightsCount, lightShaderRaw);
    m_offsets.pointCount = BaseUniformBlock::getVarOffset(LightingVars::Block::PointLightsCount, lightShaderRaw);

    m_offsets.shadowOpacity = BaseUniformBlock::getVarOffset(LightingVars::Block::ShadowOpacity, lightShaderRaw);
    m_offsets.shadowDiskRadiusK = BaseUniformBlock::getVarOffset(LightingVars::Block::ShadowDiskRadiusK, lightShaderRaw);
    m_offsets.shadowDiskRadiusMin = BaseUniformBlock::getVarOffset(LightingVars::Block::ShadowDiskRadiusMin, lightShaderRaw);

    m_dirShadowMapLoc = m_lightShader->getLocation(LightingVars::DirLightShadowMaps);
    m_pointShadowMapLoc = m_lightShader->getLocation(LightingVars::PointLightShadowMaps);

    if (m_pointShadowShader) {
        m_shadowShaderPosLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::Pos);
        m_shadowShaderFarPlaneLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::FarPlane);
        m_shadowShaderMatricesLoc = m_pointShadowShader->getLocation(ShadowShader::Vars::PointLight::ShadowMatrices);
    }

    m_offsets.dir = Array<DirLightOffsets>(m_dirLightsLimit);
    m_offsets.point = Array<PointLightOffsets>(m_pointLightsLimit);

    auto getDirLightOff = [&](uint index, const char* elem) {
        return BaseUniformBlock::getVarOffset(
            LightingVars::Block::DirLights + string("[") + to_string(index) + "]." + elem, lightShaderRaw
        );
    };

    auto getPointLightOff = [&](uint index, const char* elem) {
        return BaseUniformBlock::getVarOffset(
            LightingVars::Block::PointLights + string("[") + to_string(index) + "]." + elem, lightShaderRaw
        );
    };

    // dir light offsets
    for (uint i = 0; i < m_dirLightsLimit; ++i) {
        DirLightOffsets offsets {};
        offsets.kc = getDirLightOff(i, LightingVars::Light::Kc);
        offsets.kl = getDirLightOff(i, LightingVars::Light::Kl);
        offsets.kq = getDirLightOff(i, LightingVars::Light::Kq);
        offsets.pos = getDirLightOff(i, LightingVars::Light::Pos);
        offsets.color = getDirLightOff(i, LightingVars::Light::Color);
        offsets.minBias = getDirLightOff(i, LightingVars::Light::MinBias);
        offsets.maxBias = getDirLightOff(i, LightingVars::Light::MaxBias);
        offsets.lightMatrix = getDirLightOff(i, LightingVars::Light::LightMatrix);
        m_offsets.dir[i] = offsets;
    }

    // point light offsets
    for (uint i = 0; i < m_pointLightsLimit; ++i) {
        PointLightOffsets offsets {};
        offsets.kc = getPointLightOff(i, LightingVars::Light::Kc);
        offsets.kl = getPointLightOff(i, LightingVars::Light::Kl);
        offsets.kq = getPointLightOff(i, LightingVars::Light::Kq);
        offsets.pos = getPointLightOff(i, LightingVars::Light::Pos);
        offsets.color = getPointLightOff(i, LightingVars::Light::Color);
        offsets.far = getPointLightOff(i, LightingVars::Light::FarPlane);
        offsets.bias = getPointLightOff(i, LightingVars::Light::Bias);
        m_offsets.point[i] = offsets;
    }
}

void LightingManager::configureShadowMapping() {
    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    // Note: Mesa drivers require sampler as int, not uint

    for (int i = 0; i < m_dirLightsLimit; i++) {
        ShaderProgram::setInt(m_dirShadowMapLoc + i, static_cast<int>(m_dirLightsInitialSlot + i));
        Texture2D::getDefault()->use(m_dirLightsInitialSlot + i);
    }

    for (int i = 0; i < m_pointLightsLimit; i++) {
        ShaderProgram::setInt(m_pointShadowMapLoc + i, static_cast<int>(m_pointLightsInitialSlot + i));
        TextureCube::getDefault()->use(m_pointLightsInitialSlot + i);
    }
}

void LightingManager::setBindingPoint(uint bindingPoint) {
    m_uniformBlock.setBindingPoint(bindingPoint);
}

void LightingManager::setDirLightsLimit(uint limit) {
    m_dirLightsLimit = limit;
}

void LightingManager::setPointLightsLimit(uint limit) {
    m_pointLightsLimit = limit;
}

void LightingManager::setDirMapInitialSlot(uint slot) {
    m_dirLightsInitialSlot = slot;
}

void LightingManager::setPointMapInitialSlot(uint slot) {
    m_pointLightsInitialSlot = slot;
}

void LightingManager::setLightShader(const ShaderProgramPtr &in_lightShader) {
    m_lightShader = in_lightShader;
}

void LightingManager::setPointLightShadowShader(const ShaderProgramPtr &shadowShader) {
    m_pointShadowShader = shadowShader;
}

uint LightingManager::getDirLightsLimit() const {
    return m_dirLightsLimit;
}

uint LightingManager::getPointLightsLimit() const {
    return m_pointLightsLimit;
}

uint LightingManager::getDirMapInitialSlot() const {
    return m_dirLightsInitialSlot;
}

uint LightingManager::getPointMapInitialSlot() const {
    return m_pointLightsInitialSlot;
}

const ShaderProgramPtr& LightingManager::getLightShader() const {
    return m_lightShader;
}

const ShaderProgramPtr& LightingManager::getPointLightShadowShader() const {
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
    uint slot = m_dirLightsInitialSlot + index;
    ShaderProgram::setInt(m_dirShadowMapLoc + static_cast<int>(index), static_cast<int>(slot));
    light.getShadowMap()->use(slot);
}

void LightingManager::pushShadowMap(const PointLight &light, uint index) {
    uint slot = m_pointLightsInitialSlot + index;
    ShaderProgram::setInt(m_pointShadowMapLoc + static_cast<int>(index), static_cast<int>(slot));
    light.getShadowMap()->use(slot);
}

void LightingManager::pushShadowShaderPos(const PointLight &light) {
    ShaderProgram::setVec3(m_shadowShaderPosLoc, light.m_pos);
}

void LightingManager::pushShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::setFloat(m_shadowShaderFarPlaneLoc, light.m_far);
}

void LightingManager::pushShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++) {
        ShaderProgram::setMat4(m_shadowShaderMatricesLoc + i, light.m_lightSpaceMatrices[i]);
    }
}

void LightingManager::writeDirLightsCount(uint count) {
    m_bufferWriter.write(m_offsets.dirCount, count);
}

void LightingManager::writePointLightsCount(uint count) {
    m_bufferWriter.write(m_offsets.pointCount, count);
}

void LightingManager::writeShadowOpacity(float shadowOpacity) {
    m_bufferWriter.write(m_offsets.shadowOpacity, shadowOpacity);
}

void LightingManager::writeShadowDiskRadiusK(float diskRadiusK) {
    m_bufferWriter.write(m_offsets.shadowDiskRadiusK, diskRadiusK);
}

void LightingManager::writeShadowDiskRadiusMin(float diskRadiusMin) {
    m_bufferWriter.write(m_offsets.shadowDiskRadiusMin, diskRadiusMin);
}

void LightingManager::writeKc(const Light &light, uint index) {
    uint offset = (light.m_type == Light::Type::Dir) ? m_offsets.dir[index].kc : m_offsets.point[index].kc;
    m_bufferWriter.write(offset, light.m_kc);
}

void LightingManager::writeKl(const Light &light, uint index) {
    uint offset = (light.m_type == Light::Type::Dir) ? m_offsets.dir[index].kl : m_offsets.point[index].kl;
    m_bufferWriter.write(offset, light.m_kl);
}

void LightingManager::writeKq(const Light &light, uint index) {
    uint offset = (light.m_type == Light::Type::Dir) ? m_offsets.dir[index].kq : m_offsets.point[index].kq;
    m_bufferWriter.write(offset, light.m_kq);
}

void LightingManager::writePos(const Light &light, uint index) {
    uint offset = (light.m_type == Light::Type::Dir) ? m_offsets.dir[index].pos : m_offsets.point[index].pos;
    m_bufferWriter.write(offset, light.m_pos);
}

void LightingManager::writeColor(const Light &light, uint index) {
    uint offset = (light.m_type == Light::Type::Dir) ? m_offsets.dir[index].color : m_offsets.point[index].color;
    m_bufferWriter.write(offset, light.m_color);
}

void LightingManager::writeMinBias(const DirLight &light, uint index) {
    m_bufferWriter.write(m_offsets.dir[index].minBias, light.m_minBias);
}

void LightingManager::writeMaxBias(const DirLight &light, uint index) {
    m_bufferWriter.write(m_offsets.dir[index].maxBias, light.m_maxBias);
}

void LightingManager::writeLightMatrix(const DirLight &light, uint index) {
    m_bufferWriter.write(m_offsets.dir[index].lightMatrix, light.m_lightSpace);
}

void LightingManager::writeFarPlane(const PointLight &light, uint index) {
    m_bufferWriter.write(m_offsets.point[index].far, light.m_far);
}

void LightingManager::writeBias(const PointLight &light, uint index) {
    m_bufferWriter.write(m_offsets.point[index].bias, light.m_bias);
}
}
