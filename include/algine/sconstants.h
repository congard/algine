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
#define ALGINE_NAME_CS_LAMPS_COUNT "lampsCount"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_K "diskRadius_k"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN "diskRadius_min"
#define ALGINE_NAME_CS_SHADOW_BIAS "shadow_bias"
#define ALGINE_NAME_CS_SHADOW_OPACITY "shadowOpacity"
#define ALGINE_NAME_CS_FOCAL_DEPTH "focalDepth"
#define ALGINE_NAME_CS_FOCAL_RANGE "focalRange"
#define ALGINE_NAME_CS_SHADOW_MAPS "shadowMaps"
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
#define ALGINE_NAME_CS_LAMPS "lamps"
#define ALGINE_NAME_CS_CINEMATIC_DOF_PLANE_IN_FOCUS "cinematicDOF.p"
#define ALGINE_NAME_CS_CINEMATIC_DOF_APERTURE "cinematicDOF.a"
#define ALGINE_NAME_CS_CINEMATIC_DOF_IMAGE_DISTANCE "cinematicDOF.i"

// SS vertex shader
#define ALGINE_NAME_SS_IN_POSITION "a_Position"
#define ALGINE_NAME_SS_IN_BONE_IDS "a_BoneIds[0]"
#define ALGINE_NAME_SS_IN_BONE_WEIGHTS "a_BoneWeights[0]"
#define ALGINE_NAME_SS_MAT_MODEL "u_ModelMatrix"
#define ALGINE_NAME_SS_BONES "bones[0]"
#define ALGINE_NAME_SS_BONE_ATTRIBS_PER_VERTEX "boneAttribsPerVertex"
    
// SS geometry shader
#define ALGINE_NAME_SS_MAT_SHADOW "shadowMatrices"

// SS fragment shader
#define ALGINE_NAME_SS_LAMP_POSITION "lightPos"
#define ALGINE_NAME_SS_FAR_PLANE "far_plane"

// SSS vertex shader
#define ALGINE_NAME_SSS_IN_POSITION "inPos"
#define ALGINE_NAME_SSS_IN_TEXCOORD "inTexCoord"

// SSS fragment shader
#define ALGINE_NAME_SSS_SAMPLER_MAP_COLOR "colorMap"
#define ALGINE_NAME_SSS_SAMPLER_MAP_NORMAL "normalMap"
#define ALGINE_NAME_SSS_SAMPLER_MAP_SSRVALUES "ssrValuesMap"
#define ALGINE_NAME_SSS_SAMPLER_MAP_POSITION "positionMap"
#define ALGINE_NAME_SSS_MAT_PROJECTION "projection"
#define ALGINE_NAME_SSS_MAT_VIEW "view"
// SSR - Screen Space Reflections
#define ALGINE_NAME_SSS_SSR_SKYCOLOR "ssr_skyColor"
#define ALGINE_NAME_SSS_SSR_BINARYSEARCHCOUNT "ssr_binarySearchCount"
#define ALGINE_NAME_SSS_SSR_RAYMARCHCOUNT "ssr_rayMarchCount"
#define ALGINE_NAME_SSS_SSR_STEP "ssr_step"
#define ALGINE_NAME_SSS_SSR_LLIMITER "ssr_LLimiter"
#define ALGINE_NAME_SSS_SSR_MINRAYSTEP "ssr_minRayStep"
// BL - bloom
#define ALGINE_NAME_SSS_BL_VECTHRESHOLD "vecThreshold"
#define ALGINE_NAME_SSS_BL_BRIGHTNESSTHRESHOLD "brightnessThreshold"

// BLUS vertex shader
#define ALGINE_NAME_BLUS_IN_POSITION "inPos"
#define ALGINE_NAME_BLUS_IN_TEXCOORD "inTexCoord"
    
// BLUS fragment shader
#define ALGINE_NAME_BLUS_SAMPLER_BLOOM "image"
#define ALGINE_NAME_BLUS_SAMPLER_SCENE "scene"
#define ALGINE_NAME_BLUS_SAMPLER_DOF_BUFFER "dofBuffer"
#define ALGINE_NAME_BLUS_SIGMA_MIN "min_sigma"
#define ALGINE_NAME_BLUS_SIGMA_MAX "max_sigma"
#define ALGINE_NAME_BLUS_KERNEL_BLOOM "bloom_kernel"

// BLES vertex shader
#define ALGINE_NAME_BLES_IN_POSITION "inPos"
#define ALGINE_NAME_BLES_IN_TEXCOORD "inTexCoord"

// BLES fragment shader
#define ALGINE_NAME_BLES_SAMPLER_BLOOM_SCENE "bloomScene"
#define ALGINE_NAME_BLES_SAMPLER_DOF_SCENE "dofScene"
#define ALGINE_NAME_BLES_EXPOSURE "exposure"
#define ALGINE_NAME_BLES_GAMMA "gamma"

// lamp.cpp shader constants
#define ALGINE_NAME_CS_LAMP_LAMP_POS "lampPos"
#define ALGINE_NAME_CS_LAMP_LAMP_COLOR "lampColor"
#define ALGINE_NAME_CS_LAMP_KC "kc"
#define ALGINE_NAME_CS_LAMP_KL "kl"
#define ALGINE_NAME_CS_LAMP_KQ "kq"
#define ALGINE_NAME_CS_LAMP_FAR "far"