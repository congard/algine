/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard

  COMPILER WILL REMOVE UNUSED VARIABLES
 */

#version 330

$ALGINE_DEFINITIONS

uniform mat4 u_PVMMatrix, u_Model, u_View, u_VMMatrix;
uniform mat4 bones[MAX_BONES];
uniform float u_NormalMapping;  // Normal mapping; 0 - false, 1 - true
uniform int boneAttribsPerVertex = 0;

in vec4 a_Position; // Per-vertex position information we will pass in.
in vec4 a_BoneWeights[MAX_BONE_ATTRIBS_PER_VERTEX];
in ivec4 a_BoneIds[MAX_BONE_ATTRIBS_PER_VERTEX];
in vec3 a_Normal, a_Tangent, a_Bitangent;
in vec2 a_Texture; // Per-vertex texture information we will pass in.

out mat4 model, view, vmMatrix;
out mat3 v_TBN;
out vec4 v_Position;
out vec3 v_Normal;
out vec2 v_Texture;
out float v_NormalMapping;

#define TBN(matrix) mat3(normalize(vec3(matrix * vec4(a_Tangent, 0.0))), \
						 normalize(vec3(matrix * vec4(a_Bitangent, 0.0))), \
						 normalize(vec3(matrix * vec4(normal, 0.0))))

void main() {
    vec4 position = a_Position;
    vec3 normal = a_Normal;

    #ifdef ALGINE_BONE_SYSTEM_ENABLED
    if (boneAttribsPerVertex != 0) {
        mat4 finalTransform = mat4(0.0);
        for (int i = 0; i < boneAttribsPerVertex; i++) {
            finalTransform += bones[a_BoneIds[i].x] * a_BoneWeights[i].x;
            finalTransform += bones[a_BoneIds[i].y] * a_BoneWeights[i].y;
            finalTransform += bones[a_BoneIds[i].z] * a_BoneWeights[i].z;
            finalTransform += bones[a_BoneIds[i].w] * a_BoneWeights[i].w;
        }

        position = finalTransform * position;
        normal = mat3(finalTransform) * normal;
    }
    #endif

    #ifdef ALGINE_NORMAL_MAPPING_MODE_DUAL
	// creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
	if (u_NormalMapping == 1) v_TBN = TBN(u_VMMatrix);
    v_NormalMapping = u_NormalMapping;
    #elif defined ALGINE_NORMAL_MAPPING_MODE_ENABLED
	// creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
	v_TBN = TBN(u_VMMatrix);
    #endif

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = u_PVMMatrix * position;

    // sending all needed variables to fragment shader
    v_Position = position;
	v_Normal = normal;
    v_Texture = a_Texture;
    model = u_Model;
    view = u_View;
	vmMatrix = u_VMMatrix;
}
