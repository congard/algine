#include <algine/std/lighting/Transmitter.h>
#include <algine/std/lighting/Manager.h>

// in setters we don't using stride to get uniform location
// i.e location of myStruct[1].item = location of myStruct[0].item + elementsInStruct(myStruct)
// instead we loading it's location from shader's hash map
// because of https://stackoverflow.com/questions/57535432/glsl-array-of-struct-members-locations

#define locations manager->locations

namespace algine {
namespace Lighting {
Transmitter::Transmitter() = default;

Transmitter::Transmitter(Manager *manager) {
    this->manager = manager;
}

void Transmitter::setDirLightsCount(const uint count) {
    ShaderProgram::setUint(locations.dirLightsCount, count);
}

void Transmitter::setPointLightsCount(const uint count) {
    ShaderProgram::setUint(locations.pointLightsCount, count);
}

void Transmitter::setKc(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kc, light.m_kc);
}

void Transmitter::setKl(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kl, light.m_kl);
}

void Transmitter::setKq(const Light &light, uint index) {
    ShaderProgram::setFloat(locations.lights[light.type][index]->kq, light.m_kq);
}

void Transmitter::setPos(const Light &light, uint index) {
    ShaderProgram::setVec3(locations.lights[light.type][index]->pos, light.m_pos);
}

void Transmitter::setColor(const Light &light, uint index) {
    ShaderProgram::setVec3(locations.lights[light.type][index]->color, light.m_color);
}

void Transmitter::setShadowMap(const DirLight &light, uint index) {
    uint slot = manager->dirLightsInitialSlot + index;
    ShaderProgram::setInt(locations.lights[Light::TypeDirLight][index]->shadowMap, static_cast<int>(slot));
    light.shadowMap->use(slot);
}

void Transmitter::setMinBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<Manager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->minBias, light.m_minBias);
}

void Transmitter::setMaxBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<Manager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->maxBias, light.m_maxBias);
}

void Transmitter::setLightMatrix(const DirLight &light, uint index) {
    ShaderProgram::setMat4(reinterpret_cast<Manager::Locations::DirLight*>(
            locations.lights[Light::TypeDirLight][index])->lightMatrix, light.m_lightSpace);
}

void Transmitter::setShadowMap(const PointLight &light, uint index) {
    uint slot = manager->pointLightsInitialSlot + index;
    ShaderProgram::setInt(locations.lights[Light::TypePointLight][index]->shadowMap, static_cast<int>(slot));
    light.shadowMap->use(slot);
}

void Transmitter::setFarPlane(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<Manager::Locations::PointLight*>(
            locations.lights[Light::TypePointLight][index])->farPlane, light.m_far);
}

void Transmitter::setBias(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<Manager::Locations::PointLight*>(
            locations.lights[Light::TypePointLight][index])->bias, light.m_bias);
}

void Transmitter::setShadowShaderPos(const PointLight &light) {
    ShaderProgram::setVec3(locations.shadowShaderPos, light.m_pos);
}

void Transmitter::setShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::setFloat(locations.shadowShaderFarPlane, light.m_far);
}

void Transmitter::setShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++)
        ShaderProgram::setMat4(locations.shadowShaderMatrices + i, light.m_lightSpaceMatrices[i]);
}
}
}
