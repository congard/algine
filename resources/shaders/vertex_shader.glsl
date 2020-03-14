/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard

  COMPILER WILL REMOVE UNUSED VARIABLES
 */

#version 330

uniform mat4 MVPMatrix, modelMatrix, viewMatrix, MVMatrix;
uniform mat4 bones[MAX_BONES];
uniform bool u_NormalMapping;
uniform int boneAttribsPerVertex = 0;

in vec4 inPos; // Per-vertex position information we will pass in.
in vec4 inBoneWeights[MAX_BONE_ATTRIBS_PER_VERTEX];
in ivec4 inBoneIds[MAX_BONE_ATTRIBS_PER_VERTEX];
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inTexCoord; // Per-vertex texture information we will pass in.

out mat3 v_TBN;
out vec3 worldPosition;
out vec3 viewNormal;
out vec3 viewPosition;
out vec2 texCoord;

#define TBN mat3(normalize(vec3(MVMatrix * vec4(inTangent, 0.0))), \
				 normalize(vec3(MVMatrix * vec4(inBitangent, 0.0))), \
				 normalize(vec3(MVMatrix * vec4(normal, 0.0))))

void main() {
    vec4 position = inPos;
    vec3 normal = inNormal;

    #ifdef ALGINE_BONE_SYSTEM_ENABLED
    if (boneAttribsPerVertex != 0) {
        mat4 finalTransform = mat4(0.0);
        for (int i = 0; i < boneAttribsPerVertex; i++) {
            finalTransform += bones[inBoneIds[i].x] * inBoneWeights[i].x;
            finalTransform += bones[inBoneIds[i].y] * inBoneWeights[i].y;
            finalTransform += bones[inBoneIds[i].z] * inBoneWeights[i].z;
            finalTransform += bones[inBoneIds[i].w] * inBoneWeights[i].w;
        }

        position = finalTransform * position;
        normal = mat3(finalTransform) * normal;
    }
    #endif

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = MVPMatrix * position;

    /* Lighting module code */
    // creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
    #ifdef ALGINE_NORMAL_MAPPING_MODE_DUAL
	if (u_NormalMapping == 1) v_TBN = TBN;
    #elif defined ALGINE_NORMAL_MAPPING_MODE_ENABLED
	v_TBN = TBN;
    #endif

    // TODO: send all this data to fragment shader by default (not as module vars)?
    // sending all needed variables to fragment shader
    worldPosition = vec3(modelMatrix * position);
	viewNormal = vec3(normalize(MVMatrix * vec4(normal, 0.0))); // needs if normal mapping disabled or dual
    viewPosition = vec3(viewMatrix * vec4(worldPosition, 1.0));
    texCoord = inTexCoord;
}
