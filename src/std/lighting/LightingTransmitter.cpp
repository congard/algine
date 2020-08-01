#include <algine/std/lighting/LightingTransmitter.h>
#include <algine/std/lighting/LightingManager.h>

// in setters we don't using stride to get uniform location
// i.e location of myStruct[1].item = location of myStruct[0].item + elementsInStruct(myStruct)
// instead we loading it's location from shader's hash map
// because of https://stackoverflow.com/questions/57535432/glsl-array-of-struct-members-locations

#define locations manager->locations

namespace algine {
LightingTransmitter::LightingTransmitter() = default;

LightingTransmitter::LightingTransmitter(LightingManager *manager) {
    this->manager = manager;
}

void LightingTransmitter::setDirLightsCount(const uint count) {
    ShaderProgram::setUint(locations.dirLightsCount, count);
}

void LightingTransmitter::setPointLightsCount(const uint count) {
    ShaderProgram::setUint(locations.pointLightsCount, count);
}

void LightingTransmitter::setKc(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kc, light.m_kc);
}

void LightingTransmitter::setKl(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kl, light.m_kl);
}

void LightingTransmitter::setKq(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kq, light.m_kq);
}

void LightingTransmitter::setPos(const Light &light, uint index) {
    ShaderProgram::setVec3(locations.lights[light.type][index]->pos, light.m_pos);
}

void LightingTransmitter::setColor(const Light &light, uint index) {
    ShaderProgram::setVec3(locations.lights[light.type][index]->color, light.m_color);
}

void LightingTransmitter::setShadowMap(const DirLight &light, uint index) {
    uint slot = manager->dirLightsInitialSlot + index;
    ShaderProgram::setInt(locations.lights[Light::TypeDirLight][index]->shadowMap, static_cast<int>(slot));
    light.shadowMap->use(slot);
}

void LightingTransmitter::setMinBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<LightingManager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->minBias, light.m_minBias);
}

void LightingTransmitter::setMaxBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<LightingManager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->maxBias, light.m_maxBias);
}

void LightingTransmitter::setLightMatrix(const DirLight &light, uint index) {
    ShaderProgram::setMat4(reinterpret_cast<LightingManager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->lightMatrix, light.m_lightSpace);
}

void LightingTransmitter::setShadowMap(const PointLight &light, uint index) {
    uint slot = manager->pointLightsInitialSlot + index;
    ShaderProgram::setInt(locations.lights[Light::TypePointLight][index]->shadowMap, static_cast<int>(slot));
    light.shadowMap->use(slot);
}

void LightingTransmitter::setFarPlane(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<LightingManager::Locations::PointLight*>(
            locations.lights[Light::TypePointLight][index])->farPlane, light.m_far);
}

void LightingTransmitter::setBias(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<LightingManager::Locations::PointLight*>(
            locations.lights[Light::TypePointLight][index])->bias, light.m_bias);
}

void LightingTransmitter::setShadowShaderPos(const PointLight &light) {
    ShaderProgram::setVec3(locations.shadowShaderPos, light.m_pos);
}

void LightingTransmitter::setShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::setFloat(locations.shadowShaderFarPlane, light.m_far);
}

void LightingTransmitter::setShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++)
        ShaderProgram::setMat4(locations.shadowShaderMatrices + i, light.m_lightSpaceMatrices[i]);
}
}
