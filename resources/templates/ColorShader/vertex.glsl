/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

#version 330

#alp include "modules/BoneSystem.glsl"
#alp include "modules/NormalMapping.vert.glsl"

uniform mat4 MVPMatrix, modelMatrix, viewMatrix, MVMatrix;

in vec4 inPos; // Per-vertex position information we will pass in.
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inTexCoord; // Per-vertex texture information we will pass in.

out vec3 worldPosition;
out vec3 viewNormal;
out vec3 viewPosition;
out vec2 texCoord;

void main() {
    vec4 position = inPos;
    vec3 normal = inNormal;

    #ifdef ALGINE_BONE_SYSTEM
    if (isBonesPresent()) {
        mat4 finalTransform = getBoneTransformMatrix();
        position = finalTransform * position;
        normal = mat3(finalTransform) * normal;
    }
    #endif

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = MVPMatrix * position;

    /* Lighting module code */
    // creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
    #ifdef ALGINE_NORMAL_MAPPING_DUAL
	if (isNormalMappingEnabled())
        passTBN(MVMatrix, inTangent, inBitangent, normal);
    #elif defined ALGINE_NORMAL_MAPPING_FROM_MAP
	passTBN(MVMatrix, inTangent, inBitangent, normal);
    #endif

    // TODO: send all this data to fragment shader by default (not as module vars)?
    // sending all needed variables to fragment shader
    worldPosition = vec3(modelMatrix * position);
	viewNormal = vec3(normalize(MVMatrix * vec4(normal, 0.0))); // needs if normal mapping disabled or dual
    viewPosition = vec3(viewMatrix * vec4(worldPosition, 1.0));
    texCoord = inTexCoord;
}
