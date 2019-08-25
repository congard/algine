#ifndef ALGINE_SCONSTANTS_H
#define ALGINE_SCONSTANTS_H

/**
 * sconstants - shader constants
 */

#define constant(name, val) constexpr char name[] = val;

// TODO: merge with constants.h
//  constants.h + sconstants.h = constants.h
//  remove ALGINE_BLAH_BLAH_ENABLED_DISABLED in current constants.h
//  ...and remove sconstants.h

// TODO: modules: Lighting, SSR (note: have own values in Color Shader), BloomSearch (uses 2nd color component...),
//  BlendBloom, BlendDOF (Coming Soon)

namespace algine {
    namespace AlgineNames {
        namespace ColorShader {
            constant(ModelMatrix, "modelMatrix")
            constant(ViewMatrix, "viewMatrix")
            constant(MVPMatrix, "MVPMatrix")
            constant(MVMatrix, "MVMatrix")
            constant(Bones, "bones[0]") // zero element, so ShaderProgram.getLocation(Bones) + i, not bones[i]
            constant(BoneAttribsPerVertex, "boneAttribsPerVertex") // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)
            constant(InPos, "inPos")
            constant(InNormal, "inNormal")
            constant(InTexCoord, "inTexCoord")
            constant(InTangent, "inTangent")
            constant(InBitangent, "inBitangent")
            constant(InBoneIds, "inBoneIds[0]") // integer
            constant(InBoneWeights, "inBoneWeights[0]")

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
            constant(PointLightShadowMaps, "pointLightShadowMaps")
            constant(DirLightShadowMaps, "dirLightShadowMaps")

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

        namespace ShadowShader {
            constant(InPos, "a_Position")
            constant(InBoneIds, "a_BoneIds[0]") // integer
            constant(InBoneWeights, "a_BoneWeights[0]")
            constant(Bones, "bones[0]")
            constant(BoneAttribsPerVertex, "boneAttribsPerVertex") // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)
            constant(TransformationMatrix, "transformationMatrix")

            namespace PointLight {
                constant(ShadowMatrices, "shadowMatrices[0]") // from geometry shader
                constant(Pos, "lightPos")
                constant(FarPlane, "farPlane")
            }
        }

        // only vertex shader variables
        namespace QuadShader {
            constant(InPos, "inPos")
            constant(InTexCoord, "inTexCoord")
        }

        // Screen Space Reflections Shader, only fragment shader variables
        namespace SSRShader {
            constant(BaseImage, "baseImage")
            constant(NormalMap, "normalMap")
            constant(SSRValuesMap, "ssrValuesMap")
            constant(PositionMap, "positionMap")
            constant(ProjectionMatrix, "projection")
            constant(ViewMatrix, "view")
            constant(SkyColor, "skyColor")
            constant(BinarySearchCount, "binarySearchCount")
            constant(RayMarchCount, "rayMarchCount")
            constant(RayStep, "rayStep")
            constant(LLimiter, "LLimiter")
            constant(MinRayStep, "minRayStep")
        }

        namespace DOFShader {
            constant(BaseImage, "baseImage")
            constant(PositionMap, "positionMap")
            constant(CoCMap, "cocMap")
            constant(MinSigma, "minSigma")
            constant(MaxSigma, "maxSigma")
            constant(FocalDepth, "focalDepth")
            constant(FocalRange, "focalRange")
            constant(PlaneInFocus, "cinematicDOF.p")
            constant(Aperture, "cinematicDOF.a")
            constant(ImageDistance, "cinematicDOF.i")
            constant(BleedingMinDeltaZ, "bleedingMinDeltaZ") // Bleeding* - bleeding elimination variables
            constant(BleedingMinDeltaCoC, "bleedingMinDeltaCoC")
            constant(BleedingMaxFocusCoC, "bleedingMaxFocusCoC")
        }

        namespace BloomSearchShader {
            constant(BaseImage, "image")
            constant(Threshold, "threshold")
            constant(BrightnessThreshold, "brightnessThreshold")
        }

        namespace BlurShader {
            constant(BaseImage, "image")
            constant(Kernel, "kernel[0]")
        }

        namespace BlendBloomShader {
            constant(BaseImage, "image")
            constant(BloomImage, "bloom")
            constant(Exposure, "exposure")
            constant(Gamma, "gamma")
        }

        namespace CubemapShader {
            constant(InPos, "inPos")
            constant(TransformationMatrix, "transform")

            constant(ViewMatrix, "view")
            constant(CubemapTex, "cubemap")
            constant(Color, "color")
            constant(PosScaling, "scaling")
        }
    }
}

#undef constant
#endif
