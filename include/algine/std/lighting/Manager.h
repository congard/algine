#ifndef ALGINE_MODULE_H
#define ALGINE_MODULE_H

#include <algine/types.h>
#include <algine/std/lighting/Transmitter.h>
#include <algine/core/shader/ShaderProgram.h>

namespace algine {
namespace Lighting {
class Manager {
    friend class Transmitter;

public:
    enum {
        DirLightsCount, PointLightsCount,
        Kc, Kl, Kq, Pos, Color, ShadowMap, // common
        MinBias, MaxBias, LightMatrix, // dir
        FarPlane, Bias, ShadowShaderPos, ShadowShaderFarPlane, ShadowShaderMatrices // point
    };

    Manager();
    ~Manager();

    void indexDirLightLocations();
    void indexPointLightLocations();
    void configureShadowMapping();

    void setDirLightsLimit(uint limit);
    void setDirLightsMapInitialSlot(uint slot);
    void setPointLightsLimit(uint limit);
    void setPointLightsMapInitialSlot(uint slot);
    void setLightShader(ShaderProgram *lightShader);
    void setPointLightShadowShader(ShaderProgram *shadowShader);

    int getLocation(uint obj, uint lightType, uint lightIndex) const;

    uint getDirLightsLimit() const;
    uint getDirLightsMapInitialSlot() const;
    uint getPointLightsLimit() const;
    uint getPointLightsMapInitialSlot() const;
    ShaderProgram* getLightShader() const;
    ShaderProgram* getPointLightShadowShader() const;

public:
    Transmitter transmitter;

    uint dirLightsLimit = 8, pointLightsLimit = 8;
    uint dirLightsInitialSlot = 0, pointLightsInitialSlot = 0;
    ShaderProgram *lightShader = nullptr;
    ShaderProgram *pointShadowShader = nullptr;

private:
    struct Locations {
        int getLocation(uint obj, uint lightType, uint lightIndex) const;

        struct Light {
            int kc, kl, kq, pos, color, shadowMap;
        };

        struct DirLight: public Light {
            int minBias, maxBias, lightMatrix;
        };

        struct PointLight: public Light {
            int farPlane, bias;
        };

        int dirLightsCount = -1, pointLightsCount = -1;
        int shadowShaderPos = -1, shadowShaderFarPlane = -1, shadowShaderMatrices = -1; // point light shadow shader locations
        std::vector<Light*> lights[2]; // dir light locations, point light locations
    } locations;

    void clearDirLightsLocations();
    void clearPointLightsLocations();
};
}
}

#endif //ALGINE_MODULE_H
