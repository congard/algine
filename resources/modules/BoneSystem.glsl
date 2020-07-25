/**
 * Bone System
 * It is module, not shader
 * Add it in your shader via
 * #alp include "modules/BoneSystem.glsl"
 */

uniform BoneSystem {
    mat4 bones[MAX_BONES];
    int boneAttribsPerVertex;
};

in vec4 inBoneWeights[MAX_BONE_ATTRIBS_PER_VERTEX];
in ivec4 inBoneIds[MAX_BONE_ATTRIBS_PER_VERTEX];

#define isBonesPresent() (boneAttribsPerVertex > 0)
#define getBonesCount() boneAttribsPerVertex

mat4 getBoneTransformMatrix() {
    mat4 finalTransform = mat4(0.0);

    for (int i = 0; i < boneAttribsPerVertex; i++) {
        finalTransform += bones[inBoneIds[i].x] * inBoneWeights[i].x;
        finalTransform += bones[inBoneIds[i].y] * inBoneWeights[i].y;
        finalTransform += bones[inBoneIds[i].z] * inBoneWeights[i].z;
        finalTransform += bones[inBoneIds[i].w] * inBoneWeights[i].w;
    }

    return finalTransform;
}