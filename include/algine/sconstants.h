/**
 * sconstants - shader constants
 */

// CS vertex shader names
#define ALGINE_NAME_CS_MAT_MODEL "u_Model"
#define ALGINE_NAME_CS_MAT_VIEW "u_View"
#define ALGINE_NAME_CS_MAT_PVM "u_PVMMatrix"
#define ALGINE_NAME_CS_MAT_VM "u_VMMatrix"
#define ALGINE_NAME_CS_BONES "bones[0]"
#define ALGINE_NAME_CS_SWITCH_NORMAL_MAPPING "u_NormalMapping"
#define ALGINE_NAME_CS_BONE_ATTRIBS_PER_VERTEX "boneAttribsPerVertex"
#define ALGINE_NAME_CS_IN_POSITION "a_Position"
#define ALGINE_NAME_CS_IN_NORMAL "a_Normal"
#define ALGINE_NAME_CS_IN_TANGENT "a_Tangent"
#define ALGINE_NAME_CS_IN_BITANGENT "a_Bitangent"
#define ALGINE_NAME_CS_IN_TEXCOORD "a_Texture"
#define ALGINE_NAME_CS_IN_BONE_IDS "a_BoneIds[0]"
#define ALGINE_NAME_CS_IN_BONE_WEIGHTS "a_BoneWeights[0]"
    
// CS fragment shader names
#define ALGINE_NAME_CS_VIEW_POSITION "viewPos"
#define ALGINE_NAME_CS_POINT_LIGHTS_COUNT "pointLightsCount"
#define ALGINE_NAME_CS_DIR_LIGHTS_COUNT "dirLightsCount"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_K "diskRadius_k"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN "diskRadius_min"
#define ALGINE_NAME_CS_SHADOW_OPACITY "shadowOpacity"
#define ALGINE_NAME_CS_MATERIAL_AMBIENT_TEX "material.ambient"
#define ALGINE_NAME_CS_MATERIAL_DIFFUSE_TEX "material.diffuse"
#define ALGINE_NAME_CS_MATERIAL_SPECULAR_TEX "material.specular"
#define ALGINE_NAME_CS_MATERIAL_NORMAL_TEX "material.normal"
#define ALGINE_NAME_CS_MATERIAL_REFLECTIONSTRENGTH_TEX "material.reflectionStrength"
#define ALGINE_NAME_CS_MATERIAL_JITTER_TEX "material.jitter"
#define ALGINE_NAME_CS_MATERIAL_AMBIENT_COLOR "material.cambient"
#define ALGINE_NAME_CS_MATERIAL_DIFFUSE_COLOR "material.cdiffuse"
#define ALGINE_NAME_CS_MATERIAL_SPECULAR_COLOR "material.cspecular"
#define ALGINE_NAME_CS_MATERIAL_AMBIENT_STRENGTH "material.ambientStrength"
#define ALGINE_NAME_CS_MATERIAL_DIFFUSE_STRENGTH "material.diffuseStrength"
#define ALGINE_NAME_CS_MATERIAL_SPECULAR_STRENGTH "material.specularStrength"
#define ALGINE_NAME_CS_MATERIAL_SHININESS "material.shininess"
#define ALGINE_NAME_CS_SWITCH_TEXTURE_MAPPING "textureMappingEnabled"
#define ALGINE_NAME_CS_POINT_LIGHTS "pointLights"
#define ALGINE_NAME_CS_DIR_LIGHTS "dirLights"

// SS vertex shader
#define ALGINE_NAME_SS_IN_POSITION "a_Position"
#define ALGINE_NAME_SS_IN_BONE_IDS "a_BoneIds[0]"
#define ALGINE_NAME_SS_IN_BONE_WEIGHTS "a_BoneWeights[0]"
#define ALGINE_NAME_SS_MAT_LIGHT_SPACE "u_LightSpaceMatrix"
#define ALGINE_NAME_SS_MAT_MODEL "u_ModelMatrix"
#define ALGINE_NAME_SS_BONES "bones[0]"
#define ALGINE_NAME_SS_BONE_ATTRIBS_PER_VERTEX "boneAttribsPerVertex"
    
// SS geometry shader
#define ALGINE_NAME_SS_MAT_SHADOW "shadowMatrices[0]"

// SS fragment shader
#define ALGINE_NAME_SS_LAMP_POSITION "lightPos"
#define ALGINE_NAME_SS_FAR_PLANE "far_plane"

// SSRS vertex shader
#define ALGINE_NAME_SSRS_IN_POSITION "inPos"
#define ALGINE_NAME_SSRS_IN_TEXCOORD "inTexCoord"

// SSRS fragment shader
#define ALGINE_NAME_SSRS_SAMPLER_MAP_COLOR "colorMap"
#define ALGINE_NAME_SSRS_SAMPLER_MAP_NORMAL "normalMap"
#define ALGINE_NAME_SSRS_SAMPLER_MAP_SSRVALUES "ssrValuesMap"
#define ALGINE_NAME_SSRS_SAMPLER_MAP_POSITION "positionMap"
#define ALGINE_NAME_SSRS_MAT_PROJECTION "projection"
#define ALGINE_NAME_SSRS_MAT_VIEW "view"
// SSR - Screen Space Reflections
#define ALGINE_NAME_SSRS_SKYCOLOR "skyColor"
#define ALGINE_NAME_SSRS_BINARYSEARCHCOUNT "binarySearchCount"
#define ALGINE_NAME_SSRS_RAYMARCHCOUNT "rayMarchCount"
#define ALGINE_NAME_SSRS_STEP "step"
#define ALGINE_NAME_SSRS_LLIMITER "LLimiter"
#define ALGINE_NAME_SSRS_MINRAYSTEP "minRayStep"

// dof blur/CoC vertex shader
#define ALGINE_NAME_DOF_IN_POSITION "inPos"
#define ALGINE_NAME_DOF_IN_TEXCOORD "inTexCoord"
    
// dof blur/CoC fragment shader
#define ALGINE_NAME_DOF_SAMPLER_IMAGE "image"
#define ALGINE_NAME_DOF_SAMPLER_MAP_POSITION "positionMap"
#define ALGINE_NAME_DOF_SAMPLER_MAP_COC "cocMap"
#define ALGINE_NAME_DOF_SIGMA_MIN "min_sigma"
#define ALGINE_NAME_DOF_SIGMA_MAX "max_sigma"
#define ALGINE_NAME_DOF_FOCAL_DEPTH "focalDepth"
#define ALGINE_NAME_DOF_FOCAL_RANGE "focalRange"
#define ALGINE_NAME_DOF_CINEMATIC_DOF_PLANE_IN_FOCUS "cinematicDOF.p"
#define ALGINE_NAME_DOF_CINEMATIC_DOF_APERTURE "cinematicDOF.a"
#define ALGINE_NAME_DOF_CINEMATIC_DOF_IMAGE_DISTANCE "cinematicDOF.i"
#define ALGINE_NAME_DOF_BLEEDING_ELIM_MIN_DZ "bleedingMinDeltaZ"
#define ALGINE_NAME_DOF_BLEEDING_ELIM_MIN_DCOC "bleedingMinDeltaCoC"
#define ALGINE_NAME_DOF_BLEEDING_ELIM_MAX_FCOC "bleedingMaxFocusCoC"

// blend vertex shader
#define ALGINE_NAME_BLEND_IN_POSITION "inPos"
#define ALGINE_NAME_BLEND_IN_TEXCOORD "inTexCoord"

// blend fragment shader
#define ALGINE_NAME_BLEND_SAMPLER_BLOOM "bloom"
#define ALGINE_NAME_BLEND_SAMPLER_IMAGE "image"
#define ALGINE_NAME_BLEND_EXPOSURE "exposure"
#define ALGINE_NAME_BLEND_GAMMA "gamma"

// bloom search vertex
#define ALGINE_NAME_BLOOM_SEARCH_IN_POSITION "inPos"
#define ALGINE_NAME_BLOOM_SEARCH_IN_TEXCOORD "inTexCoord"

// bloom search fragment
#define ALGINE_NAME_BLOOM_SEARCH_THRESHOLD "threshold"
#define ALGINE_NAME_BLOOM_SEARCH_BRIGHTNESS_THRESHOLD "brightnessThreshold"
#define ALGINE_NAME_BLOOM_SEARCH_IMAGE "image"

// blur vertex
#define ALGINE_NAME_BLUR_IN_POSITION "inPos"
#define ALGINE_NAME_BLUR_IN_TEXCOORD "inTexCoord"

// blur fragment
#define ALGINE_NAME_BLUR_IMAGE "image"
#define ALGINE_NAME_BLUR_KERNEL "kernel[0]"

// cubemap vertex
#define ALGINE_NAME_CUBEMAP_IN_POSITION "inPos"
#define ALGINE_NAME_CUBEMAP_MAT_TRANSFORM "transform"

// cubemap fragment
#define ALGINE_NAME_CUBEMAP_CUBEMAP "cubemap"
#define ALGINE_NAME_CUBEMAP_COLOR "color"
#define ALGINE_NAME_CUBEMAP_POS_SCALING "scaling"

// light constants
#define ALGINE_NAME_CS_LIGHT_POS "pos"
#define ALGINE_NAME_CS_LIGHT_COLOR "color"
#define ALGINE_NAME_CS_LIGHT_KC "kc"
#define ALGINE_NAME_CS_LIGHT_KL "kl"
#define ALGINE_NAME_CS_LIGHT_KQ "kq"
#define ALGINE_NAME_CS_LIGHT_FAR "far"
#define ALGINE_NAME_CS_LIGHT_BIAS "bias"
#define ALGINE_NAME_CS_LIGHT_MAX_BIAS "maxBias"
#define ALGINE_NAME_CS_LIGHT_MIN_BIAS "minBias"
#define ALGINE_NAME_CS_LIGHT_SHADOW_MAP "shadowMap"
#define ALGINE_NAME_CS_LIGHT_MATRIX "lightMatrix"