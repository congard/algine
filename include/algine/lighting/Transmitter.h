#ifndef ALGINE_TRANSMITTER_H
#define ALGINE_TRANSMITTER_H

#include <algine/types.h>
#include <algine/light.h>

namespace algine {
namespace Lighting {
class Manager;

/**
 * Designed only for using in Manager
 * Do not create your own instance of Transmitter
 */
class Transmitter {
public:
    Transmitter();
    explicit Transmitter(Manager *manager);

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
    Manager *manager = nullptr;
};
}
}

#endif //ALGINE_TRANSMITTER_H
