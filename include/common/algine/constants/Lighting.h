#ifndef ALGINE_LIGHTING_H
#define ALGINE_LIGHTING_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace Lighting {
    namespace Settings {
        constant(Attenuation, "ALGINE_ATTENUATION_MODE_ENABLED")
        constant(PointLightsLimit, "MAX_POINT_LIGHTS_COUNT")
        constant(DirLightsLimit, "MAX_DIR_LIGHTS_COUNT")
        constant(ShadowMappingPCF, "ALGINE_SHADOW_MAPPING_MODE_ENABLED")
        constant(ShadowMapping, "ALGINE_SHADOW_MAPPING_MODE_SIMPLE")
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
}
}

#undef constant

#endif //ALGINE_LIGHTING_H
