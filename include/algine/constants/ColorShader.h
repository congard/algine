#ifndef ALGINE_COLORSHADER_H
#define ALGINE_COLORSHADER_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace ColorShader {
        namespace Settings {
            constant(Lighting, "ALGINE_LIGHTING_MODE_ENABLED")
            constant(LightingAttenuation, "ALGINE_ATTENUATION_MODE_ENABLED")
            constant(NormalMapping, "ALGINE_NORMAL_MAPPING_MODE_ENABLED")
            constant(NormalMappingSwitcher, "ALGINE_NORMAL_MAPPING_MODE_DUAL")
            constant(PointLightsLimit, "MAX_POINT_LIGHTS_COUNT")
            constant(DirLightsLimit, "MAX_DIR_LIGHTS_COUNT")
            constant(ShadowMappingPCF, "ALGINE_SHADOW_MAPPING_MODE_ENABLED")
            constant(ShadowMapping, "ALGINE_SHADOW_MAPPING_MODE_SIMPLE")
            constant(SSR, "ALGINE_SSR_MODE_ENABLED") // TODO: remove from fragment_shader.glsl
            constant(TextureMapping, "ALGINE_TEXTURE_MAPPING_MODE_ENABLED")
            constant(TextureMappingSwitcher, "ALGINE_TEXTURE_MAPPING_MODE_DUAL")
        }

        namespace Vars {
            constant(ModelMatrix, "modelMatrix")
            constant(ViewMatrix, "viewMatrix")
            constant(MVPMatrix, "MVPMatrix")
            constant(MVMatrix, "MVMatrix")
            constant(InPos, "inPos")
            constant(InNormal, "inNormal")
            constant(InTexCoord, "inTexCoord")
            constant(InTangent, "inTangent")
            constant(InBitangent, "inBitangent")

            constant(CameraPos, "cameraPos")
            constant(PointLightsCount, "pointLightsCount")
            constant(DirLightsCount, "dirLightsCount")
            constant(ShadowDiskRadiusK, "diskRadius_k")
            constant(ShadowDiskRadiusMin, "diskRadius_min")
            constant(ShadowOpacity, "shadowOpacity")

            namespace Material {
                constant(AmbientTex, "material.ambient")
                constant(DiffuseTex, "material.diffuse")
                constant(SpecularTex, "material.specular")
                constant(NormalTex, "material.normal")
                constant(ReflectionStrengthTex, "material.reflectionStrength")
                constant(JitterTex, "material.jitter")
                constant(AmbientColor, "material.cambient")
                constant(DiffuseColor, "material.cdiffuse")
                constant(SpecularColor, "material.cspecular")
                constant(AmbientStrength, "material.ambientStrength")
                constant(DiffuseStrength, "material.diffuseStrength")
                constant(SpecularStrength, "material.specularStrength")
                constant(Shininess, "material.shininess")
                constant(IsTextureMappingEnabled, "textureMappingEnabled")
                constant(IsNormalMappingEnabled, "u_NormalMapping") // from vertex shader
            }

            constant(PointLights, "pointLights")
            constant(DirLights, "dirLights")
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
        }
    }
}

#undef constant

#endif //ALGINE_COLORSHADER_H
