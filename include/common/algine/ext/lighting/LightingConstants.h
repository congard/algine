#ifndef ALGINE_LIGHTINGCONSTANTS_H
#define ALGINE_LIGHTINGCONSTANTS_H

#define constant(name, val) constexpr char name[] = val;

namespace algine::Module::Lighting {
    namespace Settings {
        constant(PointLightsLimit, "MAX_POINT_LIGHTS_COUNT")
        constant(DirLightsLimit, "MAX_DIR_LIGHTS_COUNT")
    }

    namespace Vars {
        constant(PointLightShadowMaps, "pointLightShadowMaps[0]")
        constant(DirLightShadowMaps, "dirLightShadowMaps[0]")

        namespace Light {
            constant(Pos, "pos")
            constant(Color, "color")
            constant(Kc, "kc")
            constant(Kl, "kl")
            constant(Kq, "kq")
            constant(FarPlane, "far")
            constant(Bias, "bias")
            constant(MaxBias, "maxBias")
            constant(MinBias, "minBias")
            constant(LightMatrix, "lightMatrix")
        }

        namespace Block {
            constant(Name, "Lighting")
            constant(PointLightsCount, "pointLightsCount")
            constant(DirLightsCount, "dirLightsCount")
            constant(PointLights, "pointLights")
            constant(DirLights, "dirLights")
            constant(ShadowDiskRadiusK, "diskRadius_k")
            constant(ShadowDiskRadiusMin, "diskRadius_min")
            constant(ShadowOpacity, "shadowOpacity")
        }
    }
}

#undef constant

#endif //ALGINE_LIGHTINGCONSTANTS_H
