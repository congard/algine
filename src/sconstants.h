/**
 * sconstants - shader constants
 */

// CS vertex shader names
#define ALGINE_NAME_CS_MAT_MODEL "u_Model"
#define ALGINE_NAME_CS_MAT_VIEW "u_View"
#define ALGINE_NAME_CS_MAT_PVM "u_PVMMatrix"
#define ALGINE_NAME_CS_MAT_VM "u_VMMatrix"
#define ALGINE_NAME_CS_SWITCH_NORMAL_MAPPING "u_NormalMapping"
#define ALGINE_NAME_CS_IN_POSITION "a_Position"
#define ALGINE_NAME_CS_IN_NORMAL "a_Normal"
#define ALGINE_NAME_CS_IN_TANGENT "a_Tangent"
#define ALGINE_NAME_CS_IN_BITANGENT "a_Bitangent"
#define ALGINE_NAME_CS_IN_TEXCOORD "a_Texture"
    
// CS fragment shader names
#define ALGINE_NAME_CS_VIEW_POSITION "viewPos"
#define ALGINE_NAME_CS_LAMPS_COUNT "lampsCount"
#define ALGINE_NAME_CS_BRIGHTNESS_THRESHOLD "brightnessThreshold"
#define ALGINE_NAME_CS_SHADOW_FAR_PLANE "far_plane"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_K "diskRadius_k"
#define ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN "diskRadius_min"
#define ALGINE_NAME_CS_SHADOW_BIAS "shadow_bias"
#define ALGINE_NAME_CS_FOCAL_DEPTH "focalDepth"
#define ALGINE_NAME_CS_FOCAL_RANGE "focalRange"
#define ALGINE_NAME_CS_SHADOW_MAPS "shadowMaps"
#define ALGINE_NAME_CS_SAMPLER_MAPPING_AMBIENT "mapping.ambient"
#define ALGINE_NAME_CS_SAMPLER_MAPPING_DIFFUSE "mapping.diffuse"
#define ALGINE_NAME_CS_SAMPLER_MAPPING_SPECULAR "mapping.specular"
#define ALGINE_NAME_CS_SAMPLER_MAPPING_NORMAL "mapping.normal"
#define ALGINE_NAME_CS_COLOR_MAPPING_AMBIENT "mapping.cambient"
#define ALGINE_NAME_CS_COLOR_MAPPING_DIFFUSE "mapping.cdiffuse"
#define ALGINE_NAME_CS_COLOR_MAPPING_SPECULAR "mapping.cspecular"
#define ALGINE_NAME_CS_SWITCH_TEXTURE_MAPPING "textureMappingEnabled"
#define ALGINE_NAME_CS_LAMPS "lamps"

// SS vertex shader
#define ALGINE_NAME_SS_MAT_MODEL "u_ModelMatrix"
#define ALGINE_NAME_SS_IN_POSITION "a_Position"
    
// SS geometry shader
#define ALGINE_NAME_SS_MAT_SHADOW "shadowMatrices"

// SS fragment shader
#define ALGINE_NAME_SS_LAMP_POSITION "lightPos"
#define ALGINE_NAME_SS_FAR_PLANE "far_plane"

// BLUS vertex shader
#define ALGINE_NAME_BLUS_IN_POSITION "inPos"
#define ALGINE_NAME_BLUS_IN_TEXCOORD "inTexCoord"
    
// BLUS fragment shader
#define ALGINE_NAME_BLUS_SAMPLER_BLOOM "image"
#define ALGINE_NAME_BLUS_SAMPLER_DOF "image2"
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
#define ALGINE_NAME_CS_LAMP_AMBIENT_STRENGTH "ambientStrength"
#define ALGINE_NAME_CS_LAMP_DIFFUSE_STRENGTH "diffuseStrength"
#define ALGINE_NAME_CS_LAMP_SPECULAR_STRENGTH "specularStrength"
#define ALGINE_NAME_CS_LAMP_SHININESS "shininess"
#define ALGINE_NAME_CS_LAMP_LAMP_POS "lampPos"
#define ALGINE_NAME_CS_LAMP_LAMP_COLOR "lampColor"
#define ALGINE_NAME_CS_LAMP_KC "kc"
#define ALGINE_NAME_CS_LAMP_KL "kl"
#define ALGINE_NAME_CS_LAMP_KQ "kq"