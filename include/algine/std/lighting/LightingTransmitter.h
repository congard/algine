#ifndef ALGINE_LIGHTINGTRANSMITTER_H
#define ALGINE_LIGHTINGTRANSMITTER_H

#include <algine/types.h>
#include <algine/std/lighting/Light.h>
#include <algine/std/lighting/DirLight.h>
#include <algine/std/lighting/PointLight.h>

namespace algine {
class LightingManager;

/**
 * Designed only for using in LightingManager
 * Do not create your own instance of LightingTransmitter
 */
class LightingTransmitter {
public:
    LightingTransmitter();
    explicit LightingTransmitter(LightingManager *manager);

    void setDirLightsCount(uint count);
    void setPointLightsCount(uint count);

    void setKc(const Light &light, uint index);
    void setKl(const Light &light, uint index);
    void setKq(const Light &light, uint index);
    void setPos(const Light &light, uint index);
    void setColor(const Light &light, uint index);

    void setShadowMap(const DirLight &light, uint index);
    void setMinBias(const DirLight &light, uint index);
    void setMaxBias(const DirLight &light, uint index);
    void setLightMatrix(const DirLight &light, uint index);

    void setShadowMap(const PointLight &light, uint index);
    void setFarPlane(const PointLight &light, uint index);
    void setBias(const PointLight &light, uint index);
    void setShadowShaderPos(const PointLight &light);
    void setShadowShaderFarPlane(const PointLight &light);
    void setShadowShaderMatrices(const PointLight &light);

private:
    LightingManager *manager = nullptr;
};
}

#endif //ALGINE_LIGHTINGTRANSMITTER_H
