#include <algine/std/lighting/Manager.h>
#include <algine/constants/Lighting.h>
#include <algine/constants/ShadowShader.h>

#include <iostream>

namespace LightingVars = algine::Module::Lighting::Vars;

namespace algine {
namespace Lighting {
Manager::Manager() {
    transmitter = Transmitter(this);
};

Manager::~Manager() {
    clearDirLightsLocations();
    clearPointLightsLocations();
}

void Manager::clearDirLightsLocations() {
    for (auto &i : locations.lights[0])
        delete i;
    locations.lights[0].clear();
}

void Manager::clearPointLightsLocations() {
    for (auto &i : locations.lights[1])
        delete i;
    locations.lights[1].clear();
}

#define dirLightElem(elem) \
    LightingVars::DirLights + std::string("[") + std::to_string(i) + "]." + LightingVars::Light::elem

#define pointLightElem(elem) \
    LightingVars::PointLights + std::string("[") + std::to_string(i) + "]." + LightingVars::Light::elem

void Manager::indexDirLightLocations() {
    clearDirLightsLocations();

    locations.dirLightsCount = lightShader->getLocation(LightingVars::DirLightsCount);

    locations.lights[0].reserve(dirLightsLimit);
    for (uint i = 0; i < dirLightsLimit; ++i) {
        auto lightLocations = new Locations::DirLight;
        lightLocations->kc = lightShader->getLocation(dirLightElem(Kc));
        lightLocations->kl = lightShader->getLocation(dirLightElem(Kl));
        lightLocations->kq = lightShader->getLocation(dirLightElem(Kq));
        lightLocations->pos = lightShader->getLocation(dirLightElem(Pos));
        lightLocations->color = lightShader->getLocation(dirLightElem(Color));
        lightLocations->shadowMap = lightShader->getLocation(LightingVars::DirLightShadowMaps) + i;
        lightLocations->minBias = lightShader->getLocation(dirLightElem(MinBias));
        lightLocations->maxBias = lightShader->getLocation(dirLightElem(MaxBias));
        lightLocations->lightMatrix = lightShader->getLocation(dirLightElem(LightMatrix));
        locations.lights[0].push_back(lightLocations);
    }
}

void Manager::indexPointLightLocations() {
    clearPointLightsLocations();

    locations.pointLightsCount = lightShader->getLocation(LightingVars::PointLightsCount);

    if (pointShadowShader) {
        locations.shadowShaderPos = pointShadowShader->getLocation(ShadowShader::Vars::PointLight::Pos);
        locations.shadowShaderFarPlane = pointShadowShader->getLocation(ShadowShader::Vars::PointLight::FarPlane);
        locations.shadowShaderMatrices = pointShadowShader->getLocation(ShadowShader::Vars::PointLight::ShadowMatrices);
    }

    locations.lights[1].reserve(pointLightsLimit);
    for (uint i = 0; i < pointLightsLimit; ++i) {
        auto lightLocations = new Locations::PointLight;
        lightLocations->kc = lightShader->getLocation(pointLightElem(Kc));
        lightLocations->kl = lightShader->getLocation(pointLightElem(Kl));
        lightLocations->kq = lightShader->getLocation(pointLightElem(Kq));
        lightLocations->pos = lightShader->getLocation(pointLightElem(Pos));
        lightLocations->color = lightShader->getLocation(pointLightElem(Color));
        lightLocations->shadowMap = lightShader->getLocation(LightingVars::PointLightShadowMaps) + i;
        lightLocations->farPlane = lightShader->getLocation(pointLightElem(FarPlane));
        lightLocations->bias = lightShader->getLocation(pointLightElem(Bias));
        locations.lights[1].push_back(lightLocations);
    }
}

void Manager::configureShadowMapping() {
    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    // Note: Mesa drivers require sampler as int, not uint

    for (uint i = 0; i < dirLightsLimit; i++) {
        ShaderProgram::setInt(getLocation(Manager::ShadowMap, Light::TypeDirLight, i),
                              static_cast<int>(dirLightsInitialSlot + i));
        glActiveTexture(GL_TEXTURE0 + dirLightsInitialSlot + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    for (uint i = 0; i < pointLightsLimit; i++) {
        ShaderProgram::setInt(getLocation(Manager::ShadowMap, Light::TypePointLight, i),
                              static_cast<int>(pointLightsInitialSlot + i));
        glActiveTexture(GL_TEXTURE0 + pointLightsInitialSlot + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}

void Manager::setDirLightsLimit(const uint limit) {
    dirLightsLimit = limit;
}

void Manager::setDirLightsMapInitialSlot(uint slot) {
    dirLightsInitialSlot = slot;
}

void Manager::setPointLightsLimit(const uint limit) {
    pointLightsLimit = limit;
}

void Manager::setPointLightsMapInitialSlot(uint slot) {
    pointLightsInitialSlot = slot;
}

void Manager::setLightShader(ShaderProgram *const in_lightShader) {
    lightShader = in_lightShader;
}

void Manager::setPointLightShadowShader(ShaderProgram *shadowShader) {
    pointShadowShader = shadowShader;
}

int Manager::getLocation(const uint obj, const uint lightType, const uint lightIndex) const {
    return locations.getLocation(obj, lightType, lightIndex);
}

uint Manager::getDirLightsLimit() const {
    return dirLightsLimit;
}

uint Manager::getDirLightsMapInitialSlot() const {
    return dirLightsInitialSlot;
}

uint Manager::getPointLightsLimit() const {
    return pointLightsLimit;
}

uint Manager::getPointLightsMapInitialSlot() const {
    return pointLightsInitialSlot;
}

ShaderProgram *Manager::getLightShader() const {
    return lightShader;
}

ShaderProgram *Manager::getPointLightShadowShader() const {
    return pointShadowShader;
}

#define checkIsDirLight \
    if (lightType != algine::Lighting::Light::TypeDirLight) { \
        std::cerr << "Object " << obj << " can only be used with Light::TypeDirLight\n"; \
        return -1; \
    }

#define checkIsPointLight \
    if (lightType != algine::Lighting::Light::TypePointLight) { \
        std::cerr << "Object " << obj << " can only be used with Light::TypePointLight\n"; \
        return -1; \
    }

inline int Manager::Locations::getLocation(const uint obj, const uint lightType, const uint lightIndex) const {
    switch (obj) {
        case DirLightsCount:
            return dirLightsCount;
        case PointLightsCount:
            return pointLightsCount;
        case Kc:
            return lights[lightType][lightIndex]->kc;
        case Kl:
            return lights[lightType][lightIndex]->kl;
        case Kq:
            return lights[lightType][lightIndex]->kq;
        case Pos:
            return lights[lightType][lightIndex]->pos;
        case Color:
            return lights[lightType][lightIndex]->color;
        case ShadowMap:
            return lights[lightType][lightIndex]->shadowMap;
        case MinBias:
            checkIsDirLight
            return reinterpret_cast<Locations::DirLight*>(lights[lightType][lightIndex])->minBias;
        case MaxBias:
            checkIsDirLight
            return reinterpret_cast<Locations::DirLight*>(lights[lightType][lightIndex])->maxBias;
        case LightMatrix:
            checkIsDirLight
            return reinterpret_cast<Locations::DirLight*>(lights[lightType][lightIndex])->lightMatrix;
        case Bias:
            checkIsPointLight
            return reinterpret_cast<Locations::PointLight*>(lights[lightType][lightIndex])->bias;
        case FarPlane:
            checkIsPointLight
            return reinterpret_cast<Locations::PointLight*>(lights[lightType][lightIndex])->farPlane;
        case ShadowShaderPos:
            checkIsPointLight
            return shadowShaderPos;
        case ShadowShaderFarPlane:
            checkIsPointLight
            return shadowShaderFarPlane;
        case ShadowShaderMatrices:
            checkIsPointLight
            return shadowShaderMatrices;
        default:
            std::cerr << "Object " << obj << " not found\n";
            return -1;
    }
}
}
}