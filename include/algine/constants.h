#ifndef ALGINE_CONSTANTS_H
#define ALGINE_CONSTANTS_H

#define constant(name, val) constexpr char name[] = val;

// TODO: modules: Lighting, SSR (note: have own values in Color Shader), BloomSearch (uses 2nd color component...),
//  BlendBloom, BlendDOF (Coming Soon)

namespace algine {
    namespace AlgineConstants {
        namespace ShaderManager {
            constant(Include, "include")
        }
    }

    namespace AlgineNames {
        // TODO: remove MODE_XXX
        //  rename definitions
        // TODO: modules as namespaces
        namespace ShaderDefinitions {
            constant(TextureMapping, "ALGINE_TEXTURE_MAPPING_MODE_ENABLED")
            constant(TextureMappingSwitcher, "ALGINE_TEXTURE_MAPPING_MODE_DUAL")
            constant(BoneSystem, "ALGINE_BONE_SYSTEM_ENABLED")
            constant(MaxBoneAttribsPerVertex, "MAX_BONE_ATTRIBS_PER_VERTEX")
            constant(MaxBones, "MAX_BONES")
            constant(OutputType, "vecout")
            constant(TexComponent, "texComponent")
            constant(SSR, "ALGINE_SSR_MODE_ENABLED") // TODO: remove from fragment_shader.glsl

            namespace Lighting {
                constant(Lighting, "ALGINE_LIGHTING_MODE_ENABLED")
                constant(LightingAttenuation, "ALGINE_ATTENUATION_MODE_ENABLED")
                constant(NormalMapping, "ALGINE_NORMAL_MAPPING_MODE_ENABLED")
                constant(NormalMappingSwitcher, "ALGINE_NORMAL_MAPPING_MODE_DUAL")
                constant(PointLightsLimit, "MAX_POINT_LIGHTS_COUNT")
                constant(DirLightsLimit, "MAX_DIR_LIGHTS_COUNT")
                constant(ShadowMappingPCF, "ALGINE_SHADOW_MAPPING_MODE_ENABLED")
                constant(ShadowMapping, "ALGINE_SHADOW_MAPPING_MODE_SIMPLE")
            }

            namespace ShadowShader {
                constant(PointLightShadowMapping, "ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING")
                constant(DirLightShadowMapping, "ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING")
            }

            namespace Blur {
                constant(Vertical, "ALGINE_VERTICAL")
                constant(Horizontal, "ALGINE_HORIZONTAL")
                constant(KernelRadius, "KERNEL_RADIUS")
            }

            namespace Dof {
                constant(Dof, "ALGINE_LINEAR_DOF")
                constant(CinematicDof, "ALGINE_CINEMATIC_DOF")
                constant(DofCocMap, "ALGINE_DOF_FROM_COC_MAP")
                constant(BleedingMinDeltaZ, "ALGINE_BLEEDING_ELIM_DZ")
                constant(BleedingMinDeltaCoC, "ALGINE_BLEEDING_ELIM_DCOC")
                constant(BleedingMaxFocusCoC, "ALGINE_BLEEDING_ELIM_FCOC")
            }

            namespace Bloom {
                constant(BloomAdd, "ALGINE_BLOOM_TYPE_ADD")
                constant(BloomScreen, "ALGINE_BLOOM_TYPE_SCREEN")
            }

            namespace CubemapShader {
                constant(CubePositions, "ALGINE_CUBE_POSITIONS")
                constant(SpherePositions, "ALGINE_SPHERE_POSITIONS")
                constant(ColorOut, "ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT")
                constant(PosOut, "ALGINE_POS_OUT_COLOR_COMPONENT")
            }
        }

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
#endif //ALGINE_CONSTANTS_H